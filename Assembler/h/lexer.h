#ifndef _LEXER_H
#define _LEXER_H

#include <string>

#include <list>

#include <iostream>

typedef std::string TokenType;

class Token
{
	friend class Lexer;

public:
	TokenType type;
	std::string token_string;
public:
	Token() = default;
	Token(TokenType type, std::string token_string)
	{
		this->type = type;
		this->token_string = token_string;
	}

	TokenType get_token_type()
	{
		return type;
	}

	std::string get_token_string()
	{
		return token_string;
	}
};

class Lexer
{
private:
	std::string file_content;
	unsigned int token_pointer;
	Token get_next_token();
	Token get_next_multichar_token();
	TokenType deduce_token_type(std::string& token_string);
public:
	static bool check_hex(char c);
	static int get_lines_number(std::list<Token>& list);
	static int get_line_number(std::list<Token>& list, std::list<Token>::iterator it);
	Lexer(std::string& file_content)
	{
		this->file_content = file_content;
		token_pointer = 0;
	}
	std::list<Token> get_token_list();
};
#endif
