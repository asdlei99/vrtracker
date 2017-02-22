#pragma once
#include "openvr_indexers.h"

#define TBL_SIZE(t) (sizeof(t)/sizeof(t[0]))

using namespace vr;
#include "openvr_delta_cfgtables.cpp"

bool MimeTypesIndexer::GetIndexForMimeType(const char *pchMimeType, int *index)
{
	for (int i = 0; i < mime_tbl_size; i++)
	{
		if (strcmp(mime_types[i].name, pchMimeType) == 0)
		{
			*index = i;
			return true;
		}
	}
	return false;
}
const char *MimeTypesIndexer::GetNameForIndex(int index)
{
	return mime_types[index].name;
}

int MimeTypesIndexer::GetNumMimeTypes()
{
	return mime_tbl_size;
}


void PropertiesIndexer::Init(
		const device_property_row *bool_properties, int num_bool_properties,
		const device_property_row *string_properties, int num_string_properties,
		const device_property_row *uint64_properties, int num_uint64_properties,
		const device_property_row *int32_properties, int num_int32_properties,
		const device_property_row *mat34_properties, int num_mat34_properties,
		const device_property_row *float_properties, int num_float_properties
	)
	{
		property_table[PROP_BOOL] = bool_properties;
		property_table[PROP_STRING] = string_properties;
		property_table[PROP_UINT64] = uint64_properties;
		property_table[PROP_INT32] = int32_properties;
		property_table[PROP_MAT34] = mat34_properties;
		property_table[PROP_FLOAT] = float_properties;

		enum2index[PROP_BOOL].reserve(num_bool_properties);
		for (int i = 0; i < num_bool_properties; i++)
		{
			enum2index[PROP_BOOL].insert({ (int)bool_properties[i].enum_val, i });
		}

		enum2index[PROP_STRING].reserve(num_string_properties);
		for (int i = 0; i < num_string_properties; i++)
		{
			enum2index[PROP_STRING].insert({ (int)string_properties[i].enum_val, i });
		}

		enum2index[PROP_UINT64].reserve(num_uint64_properties);
		for (int i = 0; i < num_uint64_properties; i++)
		{
			enum2index[PROP_UINT64].insert({ (int)uint64_properties[i].enum_val, i });
		}

		enum2index[PROP_INT32].reserve(num_int32_properties);
		for (int i = 0; i < num_int32_properties; i++)
		{
			enum2index[PROP_INT32].insert({ (int)int32_properties[i].enum_val, i });
		}

		enum2index[PROP_MAT34].reserve(num_mat34_properties);
		for (int i = 0; i < num_mat34_properties; i++)
		{
			enum2index[PROP_MAT34].insert({ (int)mat34_properties[i].enum_val, i });
		}
		enum2index[PROP_FLOAT].reserve(num_float_properties);
		for (int i = 0; i < num_float_properties; i++)
		{
			enum2index[PROP_FLOAT].insert({ (int)float_properties[i].enum_val, i });
		}
	}

void ApplicationsPropertiesIndexer::Init()
{
	PropertiesIndexer::Init(
		(const device_property_row*)application_bool_properties_table, TBL_SIZE(application_bool_properties_table),
		(const device_property_row*)application_string_properties_table, TBL_SIZE(application_string_properties_table),
		(const device_property_row*)application_uint64_properties_table, TBL_SIZE(application_uint64_properties_table),
		nullptr, 0,
		nullptr, 0,
		nullptr, 0);
}

void ApplicationsPropertiesIndexer::WriteToStream(EncodeStream &s)
{
		int x = 33;
		encode(x, s);
#if 0
		for (int i = 0; i < NUM_SETTING_TYPES; i++)
		{
			write_string_vector_to_stream(s, custom_sections[i]);
			write_string_vector_to_stream(s, custom_settings[i]);
		}
#endif
}

void ApplicationsPropertiesIndexer::ReadFromStream(EncodeStream &s)
{
		int x;
		decode(x, s);
		assert(x == 33);
		Init();
}


void DevicePropertiesIndexer::Init()
{
	PropertiesIndexer::Init(
		device_bool_properties_table, TBL_SIZE(device_bool_properties_table),
		device_string_properties_table, TBL_SIZE(device_string_properties_table),
		device_uint64_properties_table, TBL_SIZE(device_uint64_properties_table),
		device_int32_properties_table, TBL_SIZE(device_int32_properties_table),
		device_mat34_properties_table, TBL_SIZE(device_mat34_properties_table),
		device_float_properties_table, TBL_SIZE(device_float_properties_table)
	);
}

void DevicePropertiesIndexer::WriteToStream(EncodeStream &s)
{
	int x = 33;
	encode(x, s);
#if 0
		for (int i = 0; i < NUM_SETTING_TYPES; i++)
		{
			write_string_vector_to_stream(s, custom_sections[i]);
			write_string_vector_to_stream(s, custom_settings[i]);
		}
#endif
}

void DevicePropertiesIndexer::ReadFromStream(EncodeStream &s)
{
	int x;
	decode(x, s);
	assert(x == 33);
	Init();
}

// setting clients use string sections and string names to find them
bool SettingsIndexer::AddCustomSetting(const char *section_name_in, SectionSettingType section_type, const char *setting_name_in)
{
	auto section_iter = name2section.find(section_name_in);
	unordered_string2int::iterator field_iter;
	if (section_iter != name2section.end())
	{
		field_iter = sections[section_iter->second].typed_data[section_type].fieldname2index.find(setting_name_in);
	}

	if (section_iter == name2section.end() || field_iter == sections[section_iter->second].typed_data[section_type].fieldname2index.end())
	{
		// do sme
		custom_settings[section_type].emplace_back(setting_name_in);
		custom_sections[section_type].emplace_back(section_name_in);
		const char *setting_name = custom_settings[section_type].back().c_str();
		const char *section_name = custom_sections[section_type].back().c_str();

		int section_index;
		if (section_iter == name2section.end())
		{
			section_index = sections.size();
			name2section.insert({ section_name, section_index });
			sections.emplace_back(section_name);
		}
		else
		{
			section_index = section_iter->second;
		}

		int field_index = sections[section_index].typed_data[section_type].fieldnames.size();
		sections[section_index].typed_data[section_type].fieldnames.push_back(setting_name);
		sections[section_index].typed_data[section_type].fieldname2index.insert({ setting_name, field_index });
	}
	return true;
}

void SettingsIndexer::InitDefault()
{
	int num_sections = DEFAULT_SECTIONS_TABLE_SIZE;
	sections.resize(num_sections);
	for (int i = 0; i < DEFAULT_SECTIONS_TABLE_SIZE; i++)
	{
		const section_def_t *def = &default_section_defs[i];
		name2section[def->section_name] = i;
		sections[i].section_name = def->section_name;
		sections[i].typed_data[SETTING_TYPE_BOOL].fieldnames.resize(def->bool_size);
			for (int j = 0; j < def->bool_size; j++)
			{
				const char *field_name = def->bool_settings_ary[j];
				sections[i].typed_data[SETTING_TYPE_BOOL].fieldnames[j] = field_name;
				sections[i].typed_data[SETTING_TYPE_BOOL].fieldname2index[field_name] = j;
			}

			sections[i].typed_data[SETTING_TYPE_STRING].fieldnames.resize(def->stri_size);
			for (int j = 0; j < def->stri_size; j++)
			{
				const char *field_name = def->stri_settings_ary[j];
				sections[i].typed_data[SETTING_TYPE_STRING].fieldnames[j] = field_name;
				sections[i].typed_data[SETTING_TYPE_STRING].fieldname2index[field_name] = j;
			}

			sections[i].typed_data[SETTING_TYPE_FLOAT].fieldnames.resize(def->floa_size);
			for (int j = 0; j < def->floa_size; j++)
			{
				const char *field_name = def->floa_settings_ary[j];
				sections[i].typed_data[SETTING_TYPE_FLOAT].fieldnames[j] = field_name;
				sections[i].typed_data[SETTING_TYPE_FLOAT].fieldname2index[field_name] = j;
			}

			sections[i].typed_data[SETTING_TYPE_INT32].fieldnames.resize(def->int3_size);
			for (int j = 0; j < def->int3_size; j++)
			{
				const char *field_name = def->int32_settings_ary[j];
				sections[i].typed_data[SETTING_TYPE_INT32].fieldnames[j] = field_name;
				sections[i].typed_data[SETTING_TYPE_INT32].fieldname2index[field_name] = j;
			}
		}
	}


void SettingsIndexer::Init(
		int num_bool_settings, const char **bool_setting_sections, const char **bool_setting_names,
		int num_int32_settings, const char **int32_setting_sections, const char **int32_setting_names,
		int num_string_settings, const char **string_setting_sections, const char **string_setting_names,
		int num_float_settings, const char **float_setting_sections, const char **float_setting_names)
	{
		InitDefault();
		for (int i = 0; i < num_bool_settings; i++)
		{
			AddCustomSetting(bool_setting_sections[i], SectionSettingType::SETTING_TYPE_BOOL, bool_setting_names[i]);
		}
		for (int i = 0; i < num_int32_settings; i++)
		{
			AddCustomSetting(int32_setting_sections[i], SectionSettingType::SETTING_TYPE_INT32, int32_setting_names[i]);
		}
		for (int i = 0; i < num_string_settings; i++)
		{
			AddCustomSetting(string_setting_sections[i], SectionSettingType::SETTING_TYPE_STRING, string_setting_names[i]);
		}
		for (int i = 0; i < num_float_settings; i++)
		{
			AddCustomSetting(float_setting_sections[i], SectionSettingType::SETTING_TYPE_FLOAT, float_setting_names[i]);
		}
	}

void SettingsIndexer::WriteToStream(EncodeStream &s)
	{
		int x = 33;
		encode(x, s);
		for (int i = 0; i < NUM_SETTING_TYPES; i++)
		{
			write_string_vector_to_stream(s, custom_sections[i]);
			write_string_vector_to_stream(s, custom_settings[i]);
		}
	}

void SettingsIndexer::ReadFromStream(EncodeStream &s)
	{
		int x;
		decode(x, s);
		assert(x == 33);
		InitDefault();
		std::vector<std::string> tmp_sections;
		std::vector<std::string> tmp_settings;

		for (int i = 0; i < NUM_SETTING_TYPES; i++)
		{
			tmp_sections.clear();
			tmp_settings.clear();
			read_string_vector_from_stream(s, tmp_sections);
			read_string_vector_from_stream(s, tmp_settings);

			for (int j = 0; j < tmp_sections.size(); j++)
			{
				AddCustomSetting(tmp_sections[j].c_str(), SectionSettingType(i), tmp_settings[j].c_str());
			}
		}
	}

	
