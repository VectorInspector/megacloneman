#ifndef SETTINGS_FILE_H
#define SETTINGS_FILE_H

// The settings file reads only up to a specified amount of characters to prevent exploding the
// program with a ginormous text file. It can also use a string_view item to then make use of stack
// memory which is pretty good I think. This allows to find the setting keys and values with
// relative ease!
#include <sstream>
#include <iostream>
#include <fstream>
#include <string_view>
#include <cstring>

struct SettingsFile {
	static constexpr std::size_t max_chars_read = 0x400;
	
	SettingsFile (const char* file_path);
	bool GotoSetting (const char* key, int start_index = 0);
	
	template <class m>
	m Get (const char* key, m default_value);
	bool Get (const char* key, bool default_value);
	
	char buffer [max_chars_read];
	std::size_t key_len		= 0;
	std::size_t cur_index	= std::string::npos;
	std::stringstream data_stream;
	std::string_view data;
};

// C++ template shenanigans require this be in the header, as the cpp file by itself will not
// construct the necessary symbols that may or may not end up used by the rest of the program.
template <class m>
m SettingsFile::Get (const char* key, m default_value) {
	m result = default_value;
	
	if(GotoSetting(key)) {
		data_stream >> result;
	}
	
	return result;
}

#endif
