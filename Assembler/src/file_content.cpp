#include "file_content.h"

std::string FileContent::get_file_content()
{
	if (file->is_open())
	{
		char c = file->get();
		while (file->good()) 
		{
		  file_content+=c;
		  c = file->get();
		}
	}
	return file_content;
}