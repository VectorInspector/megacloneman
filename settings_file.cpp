#include "settings_file.h"

SettingsFile::SettingsFile (const char* file_path) {
	std::ifstream f(file_path);
	
	if(f.good()) {
		f.read(buffer, max_chars_read - 1);
		data_stream.str(buffer);
		data_stream.seekg(0, std::ios::beg);
		data_stream.clear();
		data = buffer;
		// std::cout << "good!" << std::endl;
	}
	
	else {
		// std::cout << "not good!" << std::endl;
	}
}

bool SettingsFile::GotoSetting (const char* key, int start_index) {
	
	// A negative starting index allows you to parse multiple same keys for different possible
	// values if you like.
	if(start_index < 0) {
		if(std::string::npos == cur_index) {
			start_index = 0;
		}
		
		else {
			start_index = cur_index;
		}
	}
	
	cur_index = data.find(key, start_index);
	
	if(std::string::npos != cur_index) {
		key_len = std::strlen(key);
		data_stream.clear();
		data_stream.seekg(1 + key_len + cur_index, std::ios::beg);
		return true;
	}
	
	else {
		data_stream.seekg(0, std::ios::beg);
	}
	
	return false;
}

// Specialisation for bool to make it less cumbersome to use for string -> bool conversion.
bool SettingsFile::Get (const char* key, bool default_value) {
	auto temp_res = Get <std::string> (key, default_value ? "true" : "false");
	return temp_res == "true";
}
