// in the openvr api
//
// properties are enum (an ID type) -> {bool,int,string} lookup
// settings are string -> {bool,int,string} lookup
//
//
// on the query side, both really should be hashed
//

struct mime_types
{
	const char * const name;
};

const mime_types mime_types[] =
{
	{ k_pch_MimeType_HomeApp },
	{ k_pch_MimeType_GameTheater },
};
static const int mime_tbl_size = sizeof(mime_types) / sizeof(mime_types[0]);

#define EXPANDO_APP_PROPERTY_UINT64O(x) { vr::VRApplicationProperty_ ## x ## _Uint64, #x }
const device_property_row application_uint64_properties_table[] =
{
	EXPANDO_APP_PROPERTY_UINT64O(LastLaunchTime)
};

#define EXPANDO_APP_PROPERTY_BOOLO(x) { vr::VRApplicationProperty_ ## x ## _Bool, #x }
const device_property_row application_bool_properties_table[] =
{
	EXPANDO_APP_PROPERTY_BOOLO(IsDashboardOverlay),
	EXPANDO_APP_PROPERTY_BOOLO(IsTemplate),
	EXPANDO_APP_PROPERTY_BOOLO(IsInstanced),
	EXPANDO_APP_PROPERTY_BOOLO(IsInternal),
};

#define EXPANDO_APP_PROPERTY_STRINGO(x) { vr::VRApplicationProperty_ ## x ## _String, #x }
const device_property_row application_string_properties_table[] =
{
	EXPANDO_APP_PROPERTY_STRINGO(Name),
	EXPANDO_APP_PROPERTY_STRINGO(LaunchType),
	EXPANDO_APP_PROPERTY_STRINGO(WorkingDirectory),
	EXPANDO_APP_PROPERTY_STRINGO(BinaryPath),
	EXPANDO_APP_PROPERTY_STRINGO(Arguments),
	EXPANDO_APP_PROPERTY_STRINGO(URL),
	EXPANDO_APP_PROPERTY_STRINGO(Description),
	EXPANDO_APP_PROPERTY_STRINGO(NewsURL),
	EXPANDO_APP_PROPERTY_STRINGO(ImagePath),
	EXPANDO_APP_PROPERTY_STRINGO(Source),
};

// int32 device properties
#define EXPANDO_DEVICE_PROPERTY_INT32O(x) { vr::Prop_ ## x ## _Int32 , #x }
const device_property_row device_int32_properties_table[] =
{
	EXPANDO_DEVICE_PROPERTY_INT32O(DeviceClass),
	EXPANDO_DEVICE_PROPERTY_INT32O(DisplayMCType),
	EXPANDO_DEVICE_PROPERTY_INT32O(EdidVendorID),
	EXPANDO_DEVICE_PROPERTY_INT32O(EdidProductID),
	EXPANDO_DEVICE_PROPERTY_INT32O(DisplayGCType),
	EXPANDO_DEVICE_PROPERTY_INT32O(CameraCompatibilityMode),
	EXPANDO_DEVICE_PROPERTY_INT32O(DisplayMCImageWidth),
	EXPANDO_DEVICE_PROPERTY_INT32O(DisplayMCImageHeight),
	EXPANDO_DEVICE_PROPERTY_INT32O(DisplayMCImageNumChannels),
	EXPANDO_DEVICE_PROPERTY_INT32O(Axis0Type),
	EXPANDO_DEVICE_PROPERTY_INT32O(Axis1Type),
	EXPANDO_DEVICE_PROPERTY_INT32O(Axis2Type),
	EXPANDO_DEVICE_PROPERTY_INT32O(Axis3Type),
	EXPANDO_DEVICE_PROPERTY_INT32O(Axis4Type),
	EXPANDO_DEVICE_PROPERTY_INT32O(ControllerRoleHint),
};

#define EXPANDO_DEVICE_PROPERTY_UINT64O(x) { vr::Prop_ ## x ## _Uint64 , #x }
const device_property_row device_uint64_properties_table[] =
{
	EXPANDO_DEVICE_PROPERTY_UINT64O(HardwareRevision),
	EXPANDO_DEVICE_PROPERTY_UINT64O(FirmwareVersion),
	EXPANDO_DEVICE_PROPERTY_UINT64O(FPGAVersion),
	EXPANDO_DEVICE_PROPERTY_UINT64O(VRCVersion),
	EXPANDO_DEVICE_PROPERTY_UINT64O(RadioVersion),
	EXPANDO_DEVICE_PROPERTY_UINT64O(DongleVersion),
	EXPANDO_DEVICE_PROPERTY_UINT64O(CurrentUniverseId),
	EXPANDO_DEVICE_PROPERTY_UINT64O(PreviousUniverseId),
	EXPANDO_DEVICE_PROPERTY_UINT64O(DisplayFirmwareVersion),
	EXPANDO_DEVICE_PROPERTY_UINT64O(CameraFirmwareVersion),
	EXPANDO_DEVICE_PROPERTY_UINT64O(DisplayBootloaderVersion),
	EXPANDO_DEVICE_PROPERTY_UINT64O(DisplayHardwareVersion),
	EXPANDO_DEVICE_PROPERTY_UINT64O(AudioFirmwareVersion),
	EXPANDO_DEVICE_PROPERTY_UINT64O(SupportedButtons),
	EXPANDO_DEVICE_PROPERTY_UINT64O(ParentDriver),
};

#define EXPANDO_DEVICE_PROPERTY_MAT34O(x) { vr::Prop_ ## x ## _Matrix34 , #x }
const device_property_row device_mat34_properties_table[] =
{
	EXPANDO_DEVICE_PROPERTY_MAT34O(StatusDisplayTransform),
	EXPANDO_DEVICE_PROPERTY_MAT34O(CameraToHeadTransform),
};

#define EXPANDO_DEVICE_PROPERTY_FLOATO(x) { vr::Prop_ ## x ## _Float , #x }
const device_property_row device_float_properties_table[] =
{
	EXPANDO_DEVICE_PROPERTY_FLOATO(DeviceBatteryPercentage),
	EXPANDO_DEVICE_PROPERTY_FLOATO(SecondsFromVsyncToPhotons),
	EXPANDO_DEVICE_PROPERTY_FLOATO(DisplayFrequency),
	EXPANDO_DEVICE_PROPERTY_FLOATO(UserIpdMeters),
	EXPANDO_DEVICE_PROPERTY_FLOATO(DisplayMCOffset),
	EXPANDO_DEVICE_PROPERTY_FLOATO(DisplayMCScale),
	EXPANDO_DEVICE_PROPERTY_FLOATO(DisplayGCBlackClamp),
	EXPANDO_DEVICE_PROPERTY_FLOATO(DisplayGCOffset),
	EXPANDO_DEVICE_PROPERTY_FLOATO(DisplayGCScale),
	EXPANDO_DEVICE_PROPERTY_FLOATO(DisplayGCPrescale),
	EXPANDO_DEVICE_PROPERTY_FLOATO(LensCenterLeftU),
	EXPANDO_DEVICE_PROPERTY_FLOATO(LensCenterLeftV),
	EXPANDO_DEVICE_PROPERTY_FLOATO(LensCenterRightU),
	EXPANDO_DEVICE_PROPERTY_FLOATO(LensCenterRightV),
	EXPANDO_DEVICE_PROPERTY_FLOATO(UserHeadToEyeDepthMeters),
	EXPANDO_DEVICE_PROPERTY_FLOATO(ScreenshotHorizontalFieldOfViewDegrees),
	EXPANDO_DEVICE_PROPERTY_FLOATO(ScreenshotVerticalFieldOfViewDegrees),
	EXPANDO_DEVICE_PROPERTY_FLOATO(FieldOfViewLeftDegrees),
	EXPANDO_DEVICE_PROPERTY_FLOATO(FieldOfViewRightDegrees),
	EXPANDO_DEVICE_PROPERTY_FLOATO(FieldOfViewTopDegrees),
	EXPANDO_DEVICE_PROPERTY_FLOATO(FieldOfViewBottomDegrees),
	EXPANDO_DEVICE_PROPERTY_FLOATO(TrackingRangeMinimumMeters),
	EXPANDO_DEVICE_PROPERTY_FLOATO(TrackingRangeMaximumMeters),
};

#define EXPANDO_DEVICE_PROPERTY_BOOLO(x) { vr::Prop_ ## x ## _Bool , #x }
const device_property_row device_bool_properties_table[] =
{
	// general properties that apply to all device classes
	EXPANDO_DEVICE_PROPERTY_BOOLO(WillDriftInYaw),
	EXPANDO_DEVICE_PROPERTY_BOOLO(DeviceIsWireless),
	EXPANDO_DEVICE_PROPERTY_BOOLO(DeviceIsCharging),
	EXPANDO_DEVICE_PROPERTY_BOOLO(Firmware_UpdateAvailable),
	EXPANDO_DEVICE_PROPERTY_BOOLO(Firmware_ManualUpdate),
	EXPANDO_DEVICE_PROPERTY_BOOLO(BlockServerShutdown),
	EXPANDO_DEVICE_PROPERTY_BOOLO(CanUnifyCoordinateSystemWithHmd),
	EXPANDO_DEVICE_PROPERTY_BOOLO(ContainsProximitySensor),
	EXPANDO_DEVICE_PROPERTY_BOOLO(DeviceProvidesBatteryStatus),
	EXPANDO_DEVICE_PROPERTY_BOOLO(DeviceCanPowerOff),
	EXPANDO_DEVICE_PROPERTY_BOOLO(HasCamera),
	EXPANDO_DEVICE_PROPERTY_BOOLO(Firmware_ForceUpdateRequired),
	EXPANDO_DEVICE_PROPERTY_BOOLO(ViveSystemButtonFixRequired),
	EXPANDO_DEVICE_PROPERTY_BOOLO(ReportsTimeSinceVSync),
	EXPANDO_DEVICE_PROPERTY_BOOLO(IsOnDesktop),
	EXPANDO_DEVICE_PROPERTY_BOOLO(DisplaySuppressed),
	EXPANDO_DEVICE_PROPERTY_BOOLO(DisplayAllowNightMode),
	EXPANDO_DEVICE_PROPERTY_BOOLO(UsesDriverDirectMode),
};

#define EXPANDO_DEVICE_PROPERTY_STRINGO(x) { vr::Prop_ ## x ## _String , #x }
const device_property_row device_string_properties_table[] =
{
	EXPANDO_DEVICE_PROPERTY_STRINGO(TrackingSystemName),
	EXPANDO_DEVICE_PROPERTY_STRINGO(ModelNumber),
	EXPANDO_DEVICE_PROPERTY_STRINGO(SerialNumber),
	EXPANDO_DEVICE_PROPERTY_STRINGO(RenderModelName),
	EXPANDO_DEVICE_PROPERTY_STRINGO(ManufacturerName),
	EXPANDO_DEVICE_PROPERTY_STRINGO(TrackingFirmwareVersion),
	EXPANDO_DEVICE_PROPERTY_STRINGO(HardwareRevision),
	EXPANDO_DEVICE_PROPERTY_STRINGO(AllWirelessDongleDescriptions),
	EXPANDO_DEVICE_PROPERTY_STRINGO(ConnectedWirelessDongle),
	EXPANDO_DEVICE_PROPERTY_STRINGO(Firmware_ManualUpdateURL),
	EXPANDO_DEVICE_PROPERTY_STRINGO(Firmware_ProgrammingTarget),
	EXPANDO_DEVICE_PROPERTY_STRINGO(DriverVersion),
	EXPANDO_DEVICE_PROPERTY_STRINGO(DisplayMCImageLeft),
	EXPANDO_DEVICE_PROPERTY_STRINGO(DisplayMCImageRight),
	EXPANDO_DEVICE_PROPERTY_STRINGO(DisplayGCImage),
	EXPANDO_DEVICE_PROPERTY_STRINGO(CameraFirmwareDescription),
	EXPANDO_DEVICE_PROPERTY_STRINGO(AttachedDeviceId),
	EXPANDO_DEVICE_PROPERTY_STRINGO(IconPathName),
	EXPANDO_DEVICE_PROPERTY_STRINGO(NamedIconPathDeviceOff),
	EXPANDO_DEVICE_PROPERTY_STRINGO(NamedIconPathDeviceSearching),
	EXPANDO_DEVICE_PROPERTY_STRINGO(NamedIconPathDeviceSearchingAlert),
	EXPANDO_DEVICE_PROPERTY_STRINGO(NamedIconPathDeviceReady),
	EXPANDO_DEVICE_PROPERTY_STRINGO(NamedIconPathDeviceReadyAlert),
	EXPANDO_DEVICE_PROPERTY_STRINGO(NamedIconPathDeviceNotReady),
	EXPANDO_DEVICE_PROPERTY_STRINGO(NamedIconPathDeviceStandby),
	EXPANDO_DEVICE_PROPERTY_STRINGO(NamedIconPathDeviceAlertLow),
	EXPANDO_DEVICE_PROPERTY_STRINGO(UserConfigPath),
	EXPANDO_DEVICE_PROPERTY_STRINGO(InstallPath)
};

//const char* const steamvrsection_bool_settings_ary[] =


const char* const steamvrsection_bool_settings_ary[] =
{
	vr::k_pch_SteamVR_DisplayDebug_Bool,
	vr::k_pch_SteamVR_EnableDistortion_Bool,
	vr::k_pch_SteamVR_SendSystemButtonToAllApps_Bool,
	vr::k_pch_SteamVR_BackgroundUseDomeProjection_Bool,
	vr::k_pch_SteamVR_ShowStage_Bool,
	vr::k_pch_SteamVR_ActivateMultipleDrivers_Bool,
	vr::k_pch_SteamVR_DirectMode_Bool,
	vr::k_pch_SteamVR_UsingSpeakers_Bool,
	vr::k_pch_SteamVR_BaseStationPowerManagement_Bool,
	vr::k_pch_SteamVR_NeverKillProcesses_Bool,
	vr::k_pch_SteamVR_AllowAsyncReprojection_Bool,
	vr::k_pch_SteamVR_AllowReprojection_Bool,
	vr::k_pch_SteamVR_ForceReprojection_Bool,
	vr::k_pch_SteamVR_ForceFadeOnBadTracking_Bool,
	vr::k_pch_SteamVR_ShowMirrorView_Bool,
	vr::k_pch_SteamVR_StartMonitorFromAppLaunch,
	vr::k_pch_SteamVR_StartCompositorFromAppLaunch_Bool,
	vr::k_pch_SteamVR_StartDashboardFromAppLaunch_Bool,
	vr::k_pch_SteamVR_StartOverlayAppsFromDashboard_Bool,
	vr::k_pch_SteamVR_EnableHomeApp,
	vr::k_pch_SteamVR_SetInitialDefaultHomeApp,
	vr::k_pch_SteamVR_RetailDemo_Bool,
};

const char* const steamvrsection_stri_settings_ary[] =
{
	vr::k_pch_SteamVR_RequireHmd_String,
	vr::k_pch_SteamVR_ForcedDriverKey_String,
	vr::k_pch_SteamVR_ForcedHmdKey_String,
	vr::k_pch_SteamVR_DebugProcessPipe_String,
	vr::k_pch_SteamVR_Background_String,
	vr::k_pch_SteamVR_GridColor_String,
	vr::k_pch_SteamVR_PlayAreaColor_String,
	vr::k_pch_SteamVR_MirrorViewGeometry_String,
};

const char* const steamvrsection_floa_settings_ary[] =
{
	vr::k_pch_SteamVR_IPD_Float,
	vr::k_pch_SteamVR_BackgroundCameraHeight_Float,
	vr::k_pch_SteamVR_BackgroundDomeRadius_Float,
	vr::k_pch_SteamVR_SpeakersForwardYawOffsetDegrees_Float,
	vr::k_pch_SteamVR_RenderTargetMultiplier_Float,
	vr::k_pch_SteamVR_IpdOffset_Float,
};

const char* const steamvrsection_int32_settings_ary[] =
{
	vr::k_pch_SteamVR_DisplayDebugX_Int32,
	vr::k_pch_SteamVR_DisplayDebugY_Int32,
	vr::k_pch_SteamVR_LogLevel_Int32,
	vr::k_pch_SteamVR_DirectModeEdidVid_Int32,
	vr::k_pch_SteamVR_DirectModeEdidPid_Int32,
	vr::k_pch_SteamVR_DefaultMirrorView_Int32,
};

const char* const lighthousesection_bool_settings_ary[] =
{
	vr::k_pch_Lighthouse_DisableIMU_Bool,
	vr::k_pch_Lighthouse_DBHistory_Bool
};

const char* const lighthousesection_stri_settings_ary[] =
{
	vr::k_pch_Lighthouse_UseDisambiguation_String,
};

const char* const lighthousesection_int32_settings_ary[] =
{
	vr::k_pch_Lighthouse_DisambiguationDebug_Int32,
	vr::k_pch_Lighthouse_PrimaryBasestation_Int32
};

const char* const nullsection_stri_settings_ary[] =
{
	vr::k_pch_Null_SerialNumber_String,
	vr::k_pch_Null_ModelNumber_String
};

const char* const nullsection_floa_settings_ary[] =
{
	vr::k_pch_Null_SecondsFromVsyncToPhotons_Float,
	vr::k_pch_Null_DisplayFrequency_Float
};

const char* const nullsection_int32_settings_ary[] =
{
	vr::k_pch_Null_WindowX_Int32,
	vr::k_pch_Null_WindowY_Int32,
	vr::k_pch_Null_WindowWidth_Int32,
	vr::k_pch_Null_WindowHeight_Int32,
	vr::k_pch_Null_RenderWidth_Int32,
	vr::k_pch_Null_RenderHeight_Int32
};

const char* const uisection_bool_settings_ary[] =
{
	vr::k_pch_UserInterface_StatusAlwaysOnTop_Bool,
	vr::k_pch_UserInterface_MinimizeToTray_Bool,
	vr::k_pch_UserInterface_Screenshots_Bool
};

const char* const uisection_int32_settings_ary[] =
{
	vr::k_pch_UserInterface_ScreenshotType_Int
};

const char* const notificationssection_bool_settings_ary[] =
{
	vr::k_pch_Notifications_DoNotDisturb_Bool
};

const char* const keyboardsection_bool_settings_ary[] =
{
	vr::k_pch_Keyboard_Smoothing
};

const char* const keyboardsection_floa_settings_ary[] =
{
	vr::k_pch_Keyboard_ScaleX,
	vr::k_pch_Keyboard_ScaleY,
	vr::k_pch_Keyboard_OffsetLeftX,
	vr::k_pch_Keyboard_OffsetRightX,
	vr::k_pch_Keyboard_OffsetY
};

const char* const keyboardsection_int32_settings_ary[] =
{
	vr::k_pch_Keyboard_TutorialCompletions
};

const char* const perfsection_bool_settings_ary[] =
{
	vr::k_pch_Perf_HeuristicActive_Bool,
	vr::k_pch_Perf_NotifyInHMD_Bool,
	vr::k_pch_Perf_NotifyOnlyOnce_Bool,
	vr::k_pch_Perf_AllowTimingStore_Bool,
	vr::k_pch_Perf_SaveTimingsOnExit_Bool
};

const char* const perfsection_floa_settings_ary[] =
{
	vr::k_pch_Perf_TestData_Float
};

const char* const collisionsection_bool_settings_ary[] =
{
	vr::k_pch_CollisionBounds_GroundPerimeterOn_Bool,
	vr::k_pch_CollisionBounds_CenterMarkerOn_Bool,
	vr::k_pch_CollisionBounds_PlaySpaceOn_Bool
};

const char* const collisionsection_floa_settings_ary[] =
{
	vr::k_pch_CollisionBounds_FadeDistance_Float
};

const char* const collisionsection_int32_settings_ary[] =
{
	vr::k_pch_CollisionBounds_Style_Int32,
	vr::k_pch_CollisionBounds_ColorGammaR_Int32,
	vr::k_pch_CollisionBounds_ColorGammaG_Int32,
	vr::k_pch_CollisionBounds_ColorGammaB_Int32,
	vr::k_pch_CollisionBounds_ColorGammaA_Int32
};

const char* const camerasection_bool_settings_ary[] =
{
	vr::k_pch_Camera_EnableCamera_Bool,
	vr::k_pch_Camera_EnableCameraInDashboard_Bool,
	vr::k_pch_Camera_EnableCameraForCollisionBounds_Bool,
	vr::k_pch_Camera_EnableCameraForRoomView_Bool,
};

const char* const camerasection_int32_settings_ary[] =
{
	vr::k_pch_Camera_BoundsColorGammaR_Int32,
	vr::k_pch_Camera_BoundsColorGammaG_Int32,
	vr::k_pch_Camera_BoundsColorGammaB_Int32,
	vr::k_pch_Camera_BoundsColorGammaA_Int32,
	vr::k_pch_Camera_BoundsStrength_Int32
};

const char* const audiosection_bool_settings_ary[] =
{
	vr::k_pch_audio_VIVEHDMIGain
};

const char* const audiosection_stri_settings_ary[] =
{
	vr::k_pch_audio_OnPlaybackDevice_String,
	vr::k_pch_audio_OnRecordDevice_String,
	vr::k_pch_audio_OnPlaybackMirrorDevice_String,
	vr::k_pch_audio_OffPlaybackDevice_String,
	vr::k_pch_audio_OffRecordDevice_String,
};

const char* const powersection_bool_settings_ary[] =
{
	vr::k_pch_Power_PowerOffOnExit_Bool,
	vr::k_pch_Power_AutoLaunchSteamVROnButtonPress
};

const char* const powersection_floa_settings_ary[] =
{
	vr::k_pch_Power_TurnOffScreensTimeout_Float,
	vr::k_pch_Power_TurnOffControllersTimeout_Float,
	vr::k_pch_Power_ReturnToWatchdogTimeout_Float
};

const char* const dashboardsection_bool_settings_ary[] =
{
	vr::k_pch_Dashboard_EnableDashboard_Bool,
	vr::k_pch_Dashboard_ArcadeMode_Bool
};

const char *const compositor_floa_settings_ary[] =
{
	vr::k_pch_SteamVR_RenderTargetMultiplier_Float,	// discussed here https://www.reddit.com/r/Vive/comments/59go2a/new_option_to_set_rendertargetmultipler_for/?st=izalffbw&sh=289a4f21
};


#define TBL(tablename) tablename, TBL_SIZE(tablename)
const struct section_def_t
{
	const char *const section_name;
	const char * const *bool_settings_ary;
	int bool_size;
	const char * const *stri_settings_ary;
	int stri_size;
	const char *const*floa_settings_ary;
	int floa_size;
	const char *const*int32_settings_ary;
	int int3_size;
} default_section_defs[] =
{
	{
		vr::k_pch_SteamVR_Section,
		TBL(steamvrsection_bool_settings_ary),
		TBL(steamvrsection_stri_settings_ary),
		TBL(steamvrsection_floa_settings_ary),
		TBL(steamvrsection_int32_settings_ary)
	},
	{
		k_pch_Lighthouse_Section,
		TBL(lighthousesection_bool_settings_ary),
		TBL(lighthousesection_stri_settings_ary),
		nullptr, 0,
		TBL(lighthousesection_int32_settings_ary)
	},
	{
		vr::k_pch_Null_Section,
		nullptr,0,
		TBL(nullsection_stri_settings_ary),
		TBL(nullsection_floa_settings_ary),
		TBL(nullsection_int32_settings_ary)
	},

	{
		vr::k_pch_UserInterface_Section,
		TBL(uisection_bool_settings_ary),
		nullptr, 0,
		nullptr, 0,
		TBL(uisection_int32_settings_ary)
	},
	{
		vr::k_pch_Notifications_Section,
		TBL(notificationssection_bool_settings_ary),
		nullptr, 0,
		nullptr, 0,
		nullptr, 0,
	},
	{
		vr::k_pch_Keyboard_Section,
		TBL(keyboardsection_bool_settings_ary),
		nullptr, 0,
		TBL(keyboardsection_floa_settings_ary),
		TBL(keyboardsection_int32_settings_ary)
	},
	{
		k_pch_Perf_Section,
		TBL(perfsection_bool_settings_ary),
		nullptr, 0,
		TBL(perfsection_floa_settings_ary),
		nullptr //perfsection_int32_settings_ary
	},
	{
		vr::k_pch_CollisionBounds_Section,
		TBL(collisionsection_bool_settings_ary),
		nullptr, 0, //collisionsection_string_settings_ary),
		TBL(collisionsection_floa_settings_ary),
		TBL(collisionsection_int32_settings_ary)
	},
	{
		vr::k_pch_Camera_Section,
		TBL(camerasection_bool_settings_ary),
		nullptr, 0,
		nullptr, 0,
		TBL(camerasection_int32_settings_ary)
	},
	{
		vr::k_pch_audio_Section,
		TBL(audiosection_bool_settings_ary),
		TBL(audiosection_stri_settings_ary),
		nullptr, 0,
		nullptr, 0
	},
	{
		vr::k_pch_Power_Section,
		TBL(powersection_bool_settings_ary),
		nullptr, 0,
		TBL(powersection_floa_settings_ary),
		nullptr, 0,
	},
	{
		vr::k_pch_Dashboard_Section,
		TBL(dashboardsection_bool_settings_ary),
		nullptr, 0,
		nullptr, 0,
		nullptr, 0
	},
	{
		"compositor",
		nullptr, 0,
		nullptr, 0,
		TBL(compositor_floa_settings_ary),
		nullptr, 0
	},
};

static const int DEFAULT_SECTIONS_TABLE_SIZE = sizeof(default_section_defs) / sizeof(default_section_defs[0]);
