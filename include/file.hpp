#ifndef FILE_HPP
#define FILE_HPP

using namespace std;

/// Extract contents of given file to file_content variable
/// @param file_name
/// @param file_content
/// @param file_size
/// @return true if content extracted successfully
bool extract_file(string file_name, string &file_content, int &file_size);

/// Saves file received from server
/// @param file_name
/// @param file_content
/// @param file_size
/// @return true if filed saved successfully
bool save_event_file(string file_name, string file_content, uint64_t file_size);

#endif