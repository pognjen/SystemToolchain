#ifndef _FILE_DESCRIPTOR_H
#define _FILE_DESCRIPTOR_H

#ifndef _FSTREAM_H
#define _FSTREAM_H

#include <fstream>

#endif

#include <string>

class FileContent
{
public:
	FileContent(std::string file_name)
	{
		const char* name = file_name.c_str();
		this->file = new std::ifstream();
		this->file->open(name);
		
		file_content = "";
	}
	
	std::string get_file_content();
private:
	std::ifstream* file;
	std::string file_content;
};

#endif