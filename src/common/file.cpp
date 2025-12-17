#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

#include "file.hpp"

using namespace std;

bool extract_file(string file_name, string& file_content, int& file_size)
{
    ifstream file(file_name, ios::binary);

    if (!file.is_open())
    {
        return false;
    }
    file_size = filesystem::file_size(file_name);
    
    cout << "- FILE SIZE" << file_size << '\n';
    file_content.resize(file_size);
 
    if (!file.read(&file_content[0], file_size))
    {
        return false;
    }   
    file.close(); 
    return true;
}

bool save_event_file(string file_name, string file_content, uint64_t file_size)
{
    ofstream file(file_name, ios::binary | ios::trunc);

    if (!file.is_open())
    {
        cout << "Could not open file.\n";
        return false;
    }

    file.write(file_content.c_str(), file_size);
    file.close();
    return true;
}