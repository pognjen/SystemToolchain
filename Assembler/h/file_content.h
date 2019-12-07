#ifndef _FILE_DESCRIPTOR_H
#define _FILE_DESCRIPTOR_H

#include <fstream>
#include <string>

class FileContent
{
public:
	FileContent(std::string file_name)
	{
		this->file = new std::ifstream();
		this->file->open(file_name);
		
		file_content = "";
		file_size = 0;
	}
	
	// Dumps .txt file content in string and returns string
	string get_file_content();
private:
	std::ifstream* file;
	std::string file_content;
	int file_size;
};

#endif