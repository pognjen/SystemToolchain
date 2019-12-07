#include <iostream>
#include "file_content.h"
#include "error.h"


int main(int argc,char* argv[])
{
	//parse cmd arguments
	
	FileContent file("file1.txt");
	
	std::string file_content = file.get_file_content();
	
	if (file_content == "")
	{
		Error::file_opening_error("file1.txt");
	}
	
	
	
}