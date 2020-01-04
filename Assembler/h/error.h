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
        std::cout << "Error: ] expected"<<" on line "<<line_number<<"\n";
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
	static void symbol_not_in_section(std::string symbol,int line_number)
	{
		std::cout << "Error: symbol"<<" is not defined in section on line "<<line_number<<"\n";
		exit(-1);
	}
	static void multiple_definition(std::string symbol)
	{
		std::cout << "Error: multiple definitions of symbol  "<<symbol<<"\n";
		exit(-1);
	}
	static void global_requires_operands(int line_number)
	{
		std::cout << "Error: .global(or .extern) requires operands on line "<<line_number<<"\n";
		exit(-1);
	}
	static void invalid_operand_type(int line_number)
	{
		std::cout << "Error: wrong type of operands on line "<<line_number<<"\n";
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
	static void section_operands(int line_number)
	{
		std::cout << "Error: section directive must not have operands on line"<<line_number<<"\n";
		exit(-1);
	}
	static void imported_symbol_defined(std::string symbol, int line_number)
	{
		std::cout << "Error: symbol "<<symbol<<" declared .extern and defined locally on line "<< line_number << "\n";
		exit(-1);
	}
	static void global_not_in_section(int line_number)
	{
		std::cout << "Error: .global is not defined in section on line " << line_number << "\n";
		exit(-1);
	}
	static void extern_not_in_section(int line_number)
	{
		std::cout << "Error: .extern is not defined in section on line " << line_number << "\n";
		exit(-1);
	}
	static void directive_not_in_section(std::string directive, int line_number)
	{
		std::cout << "Error: directive "<<directive<<" is not defined in section on line "<< line_number << "\n";
		exit(-1);
	}
	static void directive_requires_one_operand(std::string directive, int line_number)
	{
		std::cout << "Error: directive " << directive << " requires at least one operand on line " << line_number << "\n";
		exit(-1);
	}
	static void section_requires_one_operand(int line_number)
	{
		std::cout << "Error: section requires name on line " << line_number << "\n";
		exit(-1);
	}
	static void directive_requires_two_operands(std::string directive, int line_number)
	{
		std::cout << "Error: directive " <<directive<<" requires two operands on line " << line_number << "\n";
		exit(-1);
	}
	static void wrong_number_of_operands(std::string instruction, int line_number)
	{
		std::cout << "Error: instruction " << instruction << " has wrong number of operands on line " << line_number << "\n";
		exit(-1);
	}
	static void symbol_not_defined(std::string symbol)
	{
		std::cout << "Error: symbol " << symbol << " declared .global but was not defined\n";
		exit(-1);
	}
	static void immediate_must_not_be_dst(int line_number)
	{
		std::cout << "Error: there can not be immediate value as destination operand on line " << line_number << "\n";
		exit(-1);
	}
	static void writing_forbiden(std::string section)
	{
		std::cout << "Error: you cannot write in section " << section << "\n";
		exit(-1);
	}
	static void wrong_operands_numbers(std::string instr,int num_operands,int line_number)
	{
		std::cout << "Error: instruction " << instr << " requires " << num_operands << " on line " << line_number << '\n';
		exit(-1);
	}
	static void execution_forbidden(std::string section)
	{
		std::cout << "Error: section " <<section<<"is forbidden for execution\n";
		exit(-1);
	}
	static void wrong_operands_size(int line_number)
	{
		std::cout << "Error: wrong size of operands on line " << line_number<< "\n";
		exit(-1);
	}
	static void operands_error(int line_number)
	{
		std::cout << "Error: operands error on line " << line_number << "\n";
		exit(-1);
	}
	static void pcrel_only_jump_call(int line_number)
	{
		std::cout << "Error: pc-relative addressing only with jump or call on line " << line_number << "\n";
		exit(-1);
	}
	static void jump_call_immed_pcrel(int line_number)
	{
		std::cout << "Error: jump or call only allowed pc-relative od symbol-immediate addressing on line " << line_number << "\n";
		exit(-1);
	}
};

#endif
