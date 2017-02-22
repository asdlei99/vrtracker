#pragma once
#include "openvr_hash.h"
#include "openvr_serialization.h"


class MimeTypesIndexer
{
public:
	bool GetIndexForMimeType(const char *pchMimeType, int *index);
	const char *GetNameForIndex(int index);
	int GetNumMimeTypes();
};

struct device_property_row
{
	int enum_val;
	const char *name;
};

class PropertiesIndexer
{
public:
	enum PropertySettingType
	{
		PROP_BOOL,
		PROP_STRING,
		PROP_UINT64,
		PROP_INT32,
		PROP_MAT34,
		PROP_FLOAT,
		NUM_PROP_TYPES,
	};

	// i want properties to have names
	// i want them to have indexes into node arrays
	// properties have enum values associated with them
	// properties don't have sections
	void Init(
		const device_property_row *bool_properties, int num_bool_properties,
		const device_property_row *string_properties, int num_string_properties,
		const device_property_row *uint64_properties, int num_uint64_properties,
		const device_property_row *int32_properties, int num_int32_properties,
		const device_property_row *mat34_properties, int num_mat34_properties,
		const device_property_row *float_properties, int num_float_properties
	);

	void AddCustomProperties(
		int num_bool_properties, const char **bool_names, int *bool_values,
		int num_string_properties, const char **string_names, int *string_values,
		int num_uint64_properties, const char **uint64_names, int *uint64_values,
		int num_int32_properties, const char **int32_names, int *int32_values,
		int num_mat34_properties, const char **mat34_names, int *mat34_values,
		int num_float_properties, const char **float_names, int *float_values
	);

	void AddCustomProperty(PropertySettingType prop_type, const char *name, int val);

	bool GetIndexForEnum(PropertySettingType setting_type, int enum_val, int *index)
	{
		bool rc = false;
		auto iter = enum2index[setting_type].find(enum_val);
		if (iter != enum2index[setting_type].end())
		{
			*index = iter->second;
			rc = true;
		}
		return rc;
	}

	int GetNumPropertiesOfType(PropertySettingType setting_type)
	{
		return enum2index[setting_type].size();
	}
	int GetEnumVal(PropertySettingType setting_type, int index)
	{
		if (index < default_property_table[setting_type].size)
			return default_property_table[setting_type].rows[index].enum_val;
		else
			return custom_values[setting_type][index - default_property_table[setting_type].size];
	}
	const char* GetName(PropertySettingType setting_type, int index)
	{
		if (index < default_property_table[setting_type].size)
			return default_property_table[setting_type].rows[index].name;
		else
			return custom_names[setting_type][index - default_property_table[setting_type].size].c_str();
	}

	void WriteToStream(EncodeStream &s);
	void ReadFromStream(EncodeStream &s);

private:
	void HashDefaultRows(PropertySettingType setting_type);
	void AddCustomPropertiesArray(PropertySettingType setting_type, int num, const char **names, int *values);

	struct
	{
		const device_property_row *rows;
		int size;
	} default_property_table[NUM_PROP_TYPES];

	std::unordered_map<int, int> enum2index[NUM_PROP_TYPES];
	
	std::vector<std::string> custom_names[NUM_PROP_TYPES];
	std::vector<int> custom_values[NUM_PROP_TYPES];
};

class ApplicationsPropertiesIndexer : public PropertiesIndexer
{
public:
	void Init();
	void WriteToStream(EncodeStream &s);
	void ReadFromStream(EncodeStream &s);
};

class DevicePropertiesIndexer : public PropertiesIndexer
{
public:
	void Init(
		int num_bool_properties,	const char **bool_names,	int *bool_values,
		int num_string_properties,	const char **string_names,	int *string_values,
		int num_uint64_properties,	const char **uint64_names,	int *uint64_values,
		int num_int32_properties,	const char **int32_names,	int *int32_values,
		int num_mat34_properties,	const char **mat34_names,	int *mat34_values,
		int num_float_properties,	const char **float_names,	int *float_values
	);
	void WriteToStream(EncodeStream &s);
	void ReadFromStream(EncodeStream &s);
};

// setting clients use string sections and string names to find them
class SettingsIndexer
{
public:
	enum SectionSettingType
	{
		SETTING_TYPE_BOOL,
		SETTING_TYPE_STRING,
		SETTING_TYPE_FLOAT,
		SETTING_TYPE_INT32,
		NUM_SETTING_TYPES
	};

	std::vector < std::string > custom_sections[NUM_SETTING_TYPES];
	std::vector < std::string > custom_settings[NUM_SETTING_TYPES];

	bool AddCustomSetting(const char *section_name_in, SectionSettingType section_type, const char *setting_name_in);
	

	void Init(
		int num_bool_settings, const char **bool_setting_sections, const char **bool_setting_names,
		int num_int32_settings, const char **int32_setting_sections, const char **int32_setting_names,
		int num_string_settings, const char **string_setting_sections, const char **string_setting_names,
		int num_float_settings, const char **float_setting_sections, const char **float_setting_names);
	
	void WriteToStream(EncodeStream &s);
	void ReadFromStream(EncodeStream &s);

	bool GetIndexes(const char *section_name, SectionSettingType setting_type, const char *setting_name,
		int *section_in, int *setting_in)
	{
		bool rc = false;
		auto iter = name2section.find(section_name);
		if (iter != name2section.end())
		{
			int section = iter->second;
			auto field_iter = sections[section].typed_data[setting_type].fieldname2index.find(setting_name);
			if (field_iter != sections[section].typed_data[setting_type].fieldname2index.end())
			{
				*section_in = section;
				*setting_in = field_iter->second;
				rc = true;
			}
		}
		return rc;
	}

	const char *GetSectionName(int section)
	{
		return sections[section].section_name;
	}

	int GetNumSections()
	{
		return (int)sections.size();
	}

	int GetNumFields(const char *section_name, SectionSettingType setting_type)
	{
		int rc = 0;
		auto iter = name2section.find(section_name);
		if (iter != name2section.end())
		{
			int section = iter->second;
			rc = sections[section].typed_data[setting_type].fieldnames.size();
		}
		return rc;
	}
	const char **GetFieldNames(const char *section_name, SectionSettingType setting_type)
	{
		const char **rc = nullptr;
		auto iter = name2section.find(section_name);
		if (iter != name2section.end())
		{
			int section = iter->second;
			rc = &sections[section].typed_data[setting_type].fieldnames.at(0);
		}
		return rc;
	}
private:
	void InitDefault();
	// settings of a type
	struct subtable
	{
		std::vector<const char *> fieldnames;
		unordered_string2int fieldname2index;
	};

	struct section_data
	{
		section_data() {}
		section_data(const char *s) : section_name(s) {}
		const char *section_name;
		subtable typed_data[NUM_SETTING_TYPES];
	};

	std::vector<section_data> sections;
	unordered_string2int name2section;
};
