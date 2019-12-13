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
	static void symbol_not_in_section(std::string symbol="",int line_number)
	{
		std::cout << "Error: symbol "<<symbol<<" is not defined in section on line "<<line_number<<"\n";
		exit(-1);
	}
	static void multiple_definition(std::string symbol)
	{
		std::cout << "Error: multiple definitions of symbol  "<<symbol<<"\n";
		exit(-1);
	}
	static void Error::global_requires_operands(int line_number)
	{
		std::cout << "Error: .global(or .extern) requires operands on line "<<line_number<<"\n";
		exit(-1);
	}
	static void invalid_operand_type(int line_number)
	{
		std::cout << "Error: .global requires operands on line "<<line_number<<"\n";
		exit(-1);
	}
	static void skip_requires_one_operand(int line_number)
	{
		std::cout << "Error: .skip requires one operands on line "<<line_number<<"\n";
		exit(-1);
	}
	static void skip_requires_one_operand(int line_number)
	{
		std::cout << "Error: .skip requires one operands on line "<<line_number<<"\n";
		exit(-1);
	}
	static void skip_requires_immediate_value(int line_number)
	{
		std::cout << "Error: .skip requires immediate value on line "<<line_number<<"\n";
		exit(-1);
	}
};

#endif
