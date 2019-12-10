#ifndef _ERROR_H
#define _ERROR_H

#include <string>
#include <iostream>
class Error
{
public:
    static void file_opening_error(std::string file_name)
    {
        std::cout << "Error: " << file_name << " could not be opened\n";
        exit(-1);
    }
    static void unknown_token_type(int line_number)
    {
        std::cout << "Error: unknown token on line " << line_number << '\n';
        exit(-1);
    }
    static void colon_expected(std::string symbol,int line_number)
    {
        std::cout << "Error: colon exprected after " << symbol <<" on line "<<line_number<<"\n";
        exit(-1);
    }
    static void literal_expected(int line_number)
    {
        std::cout << "Error: literal expected"<<" on line "<<line_number<<"\n";
        exit(-1);
    }
    static void wrong_literal_width(int line_number)
    {
        std::cout << "Error: wrong witdh of literal"<<" on line "<<line_number<<"\n";
        exit(-1);
    }

    static void symbol_expected_after_ampersand(int line_number)
    {
        std::cout << "Error: symbol expected after &"<<" on line "<<line_number<<"\n";
        exit(-1);
    }
    static void symbol_or_literal_expected(int line_number)
    {
        std::cout << "Error: symbol or literal expected"<<" on line "<<line_number<<"\n";
        exit(-1);
    }
    static void right_bracket_expected(int line_number)
    {
        std::cout << "Error: [ expected"<<" on line "<<line_number<<"\n";
        exit(-1);
    }
    static void symbol_expected_after_dollar(int line_number)
    {
        std::cout << "Error: symbol expected after $"<<" on line "<<line_number<<"\n";
        exit(-1);
    }
    static void invalid_statement(int line_number)
    {
        std::cout << "Error: invalid_statement"<<" on line "<<line_number<<"\n";
        exit(-1);
    }
	static void symbol_start(std::string symbol,int line_number)
	{
		std::cout << "Error: symbol "<<symbol<<" does not start with letter on line "<<line_number<<"\n";
        exit(-1);
	}
};

#endif
