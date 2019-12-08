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
	static void unknown_token_type(int line_number)
	{
		std::cout<<"Error: unknown token on line "<<line_number<<'\n';
		exit(-1);
	}
	static void colon_expected(int line_number,std::string symbol)
	{
		std::cout<<"Error: colon exprected after "<<symbol<<" on line "<<line number<<'\n';
	}
};

#endif
