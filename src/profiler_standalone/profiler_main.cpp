// ImGui - standalone example application for SDL2 + OpenGL
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.

#include <imgui.h>
#include <imgui_internal.h>
#include "imgui_impl_sdl_gl3.h"
#include <stdio.h>
#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <SDL.h>
#include <vrdelta.h>
#include <string>
#include <vector>
#include "openvr_broker.h"
#include "tinyfiledialogs.h"

#if 0
void virtual_memory_play()
{
	LPVOID lpvBase;               // Base address of the test memory
	LPTSTR lpPtr;                 // Generic character pointer
	BOOL bSuccess;                // Flag
	DWORD i;                      // Generic counter
	SYSTEM_INFO sSysInfo;         // Useful information about the system

	GetSystemInfo(&sSysInfo);     // Initialize the structure.

	DWORD dwPageSize;               // Page size on this computer
	dwPageSize = sSysInfo.dwPageSize;

	// Reserve pages in the virtual address space of the process.
	// let's say I want 1 gig
	DWORD dwSize = 1224 * 1024 * 1024;

	lpvBase = VirtualAlloc(
		NULL,               // System selects address
		dwSize,				// Size of allocation
		MEM_COMMIT,        // Allocate reserved pages
		PAGE_NOACCESS);     // Protection = no access
	
	//*((int *)lpvBase) = 42;

	lpvBase = VirtualAlloc(
		(LPVOID)lpvBase,	// System selects address
		dwSize,				// Size of allocation
		MEM_COMMIT,        // Allocate reserved pages
		PAGE_READWRITE);     
	*((int *)lpvBase) = 42;
}
#endif

openvr_broker::open_vr_interfaces *init_openvr()
{
#ifdef HAVE_OPEN_VR_RAW
	char *error;
	openvr_broker::open_vr_interfaces *ret = new openvr_broker::open_vr_interfaces;
	if (!openvr_broker::acquire_interfaces("raw", ret, &error))
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "VR_Init Failed", error, NULL);
		delete ret; 
		ret = nullptr;
	}
	return ret;
#else
	return nullptr;
#endif
}

void update_openvr(openvr_broker::open_vr_interfaces *vr_interface, vr_state_tracker_t tracker)
{
	if (vr_interface)
	{
		vr::VREvent_t event;
		memset(&event, 7, sizeof(event));
		while (vr_interface->sysi->PollNextEvent(&event, sizeof(event)))
		{
			capture_vr_event(tracker, event);
			memset(&event, 7, sizeof(event));
		}
		capture_vr_state(tracker, *vr_interface);
	}
}

struct UpdateJob
{
	openvr_broker::open_vr_interfaces *interfaces;
	vr_state_tracker_t					vr_state_tracker; 
};

DWORD WINAPI updater_thread(_In_ LPVOID lpParameter)
{
	UpdateJob *job = (UpdateJob *)lpParameter;
	while (1)
	{
		capture_vr_state(job->vr_state_tracker, *job->interfaces);
	}
}

extern vr_state_tracker_t tracker;
int boing;

struct profiler_guis
{
	struct grid_and_title
	{
		grid_and_title(const char *t, timeline_grid_t g)
			:title(t), grid(g)
		{}
		const char *title;
		timeline_grid_t grid;
	};

	gui_context_t m_gui_context;
	std::vector<grid_and_title> timeline_windows;


	profiler_guis()
	{
		m_gui_context = create_gui_context();
	}

	void add_grid(grid_config &config, vr_state_tracker_t state)
	{
		timeline_grid_t grid = create_timeline_grid(m_gui_context, state, config);
		timeline_windows.emplace_back(config.title, grid);
	}
	void destroy_all()
	{
		if (m_gui_context != nullptr)
		{
			for (auto iter = timeline_windows.begin(); iter != timeline_windows.end(); iter++)
			{
				destroy_timeline_grid(iter->grid);
			}
			timeline_windows.clear();
			destroy_gui_context(m_gui_context);
			m_gui_context = nullptr;
		}
	}

	void reset()
	{
		destroy_all();
		m_gui_context = create_gui_context();
	}
	
	void update()
	{
		int i = 0;
		for (auto iter = timeline_windows.begin(); iter != timeline_windows.end();)
		{
			i++;
			ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
			bool open = true;
			grid_and_title &g = *iter;
			char szhack[256];
			snprintf(szhack, sizeof(szhack), "%s##%d", g.title, i);
			if (ImGui::Begin(szhack, &open))
			{
				timeline_grid_update(g.grid);
			}
			ImGui::End();
			if (open == false)
			{
				iter = timeline_windows.erase(iter);
			}
			else
			{
				iter++;
			}
		}
		gui_context_update(m_gui_context);
	}
};
	

int profiler_main()
{
//	virtual_memory_play();

	UpdateJob job;

	job.interfaces = nullptr;

#ifdef STUBBY2_EXPORTS
	job.vr_state_tracker = tracker;
#else
	TrackerConfig c;
	c.set_default();
	job.vr_state_tracker = create_vr_state_tracker(c);
#endif

#if 0
	// cursor test
	vr_state_tracker_t cursor_file = load_vrstate_from_file("c:\\vr_streams\\profiler_default.bin");
	vr_cursor_t cursor = create_cursor(cursor_file);
	openvr_broker::open_vr_interfaces cursor_interfaces = get_cursor_interfaces(cursor);
	uint32_t width;
	uint32_t height;
	cursor_interfaces.sysi->GetRecommendedRenderTargetSize(&width, &height);
#endif



    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Setup window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    SDL_Window *window = SDL_CreateWindow("ImGui SDL2+OpenGL3 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    gl3wInit();

    // Setup ImGui binding
    ImGui_ImplSdlGL3_Init(window);

    // Load Fonts
    // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
    //ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyClean.ttf", 13.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

    bool show_test_window = true;
    bool show_all_state = false;
	bool show_timeline_grid = true;
    ImVec4 clear_color = ImColor(114, 144, 154);
	bool update_now = false;
	bool constant_update = false;

	profiler_guis m_guis;
	
    // Main loop
	bool profiler_done = false;
    
    while (!profiler_done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSdlGL3_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                profiler_done = true;
        }
        ImGui_ImplSdlGL3_NewFrame(window);

		// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
		if (show_test_window)
		{
			ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
			ImGui::ShowTestWindow(&show_test_window);
		}

        // 1. Show a simple window
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
        {
            static float f = 0.0f;
            ImGui::Text("Hello, world!");
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            ImGui::ColorEdit3("clear color", (float*)&clear_color);
			if (ImGui::Button("Boing"))
			{
				boing = 1;
			}
            if (ImGui::Button("Test Window")) show_test_window ^= 1;
			if (ImGui::Button("Add fake setting"))
			{
				update_vr_config_int32_setting(job.vr_state_tracker, "goo", "doo");
			}
            if (ImGui::Button("AllState")) show_all_state ^= 1;
			if (ImGui::Button("LoadState"))
			{
				char const * lFilterPatterns[] = { "*.bin" };
				char const * lTheOpenFileName;
				lTheOpenFileName = tinyfd_openFileDialog(
					"Load...",
					"C:\\vr_streams\\",
					1,
					lFilterPatterns,
					NULL,
					1);
				if (lTheOpenFileName)
				{
					vr_state_tracker_t new_file = load_vrstate_from_file(lTheOpenFileName);
					job.vr_state_tracker = new_file;
				}
			}
			if (ImGui::Button("SaveState"))
			{
				char const * lFilterPatterns[] = { "*.bin" };
				char const * lTheSaveFileName;
				lTheSaveFileName = tinyfd_saveFileDialog(
					"Save...",
					"C:\\vr_streams\\",
					1,
					lFilterPatterns,
					NULL);
				if (lTheSaveFileName)
				{
					save_vrstate_to_file(job.vr_state_tracker, lTheSaveFileName, true);	
				}
				
			}
			if (ImGui::Button("Discrete Data"))
			{
				grid_config config;
				config.title = "Discrete Data";
				config.include_filter = "";
				config.exclude_filter = "pose|last_vsync|cumulative_stats|frame_timing";

				m_guis.add_grid(config, job.vr_state_tracker);
			}
			if (ImGui::Button("Continuous Data"))
			{
				grid_config config;
				config.title = "Continuous Data";
				config.include_filter = "events|pose|last_vsync|cumulative_stats|frame_timing";
				config.exclude_filter = "";
				m_guis.add_grid(config, job.vr_state_tracker);
			}

#ifdef HAVE_OPEN_VR_RAW
			if (ImGui::Button("VRConnect"))
			{
				static int init_attempts;
				init_attempts++;
				job.interfaces = init_openvr();
			}
			if (ImGui::Button("Update Once"))
			{
				update_now = true;
			}
			if (ImGui::Button("Update Every Tick"))
			{
				constant_update = !constant_update;
			}
			if (update_now || constant_update)
			{
				update_openvr(job.interfaces, job.vr_state_tracker);
				update_now = false;
			}
			if (ImGui::Button("Launch Update Thread"))
			{
				CreateThread(0, 0, &updater_thread, &job, 0, 0);
			}

			if (ImGui::Button("Reset"))
			{
				TrackerConfig c;
				c.set_default();
				job.vr_state_tracker = create_vr_state_tracker(c);
				m_guis.reset();
			}
#endif

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }

        // 2. Show another simple window, this time using an explicit Begin/End pair
        if (show_all_state)
        {
            ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
            bool is_up = ImGui::Begin("All State", &show_all_state);
			if (is_up)
			{
//				img_gui_update(state_tracker);
			}
            ImGui::End();
        }

		m_guis.update();
		

        // Rendering
        glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        SDL_GL_SwapWindow(window);
    }

	

#ifdef STUBBY2_EXPORTS
	// someone else needs to destroy the state tracker when embedded
	
#else
	destroy_vr_state_tracker(job.vr_state_tracker);
#endif
	
	job.vr_state_tracker = nullptr;

	delete job.interfaces;
	job.interfaces  = nullptr;

    // Cleanup
    ImGui_ImplSdlGL3_Shutdown();
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();
	
    return 0;
}


int main(int, char**)
{
	return profiler_main(); 
}