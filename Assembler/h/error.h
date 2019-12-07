#ifndef _ERROR_H
#define _ERROR_H

#include <string>
#include <iostream>
class Error
{
public:
	static void file_opening_error(std::string file_name)
	{
		std::cout<<"Error: "<<file_name<<" could not be opened\n";
		exit(-1);
	}
};

#endif