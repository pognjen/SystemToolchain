#include "lexer.h"
#include "error.h"
#include <unordered_map>
#include <unordered_set>

extern std::unordered_map<std::string, int>  instructions_map;
extern std::unordered_set<std::string> directives_map;


bool Lexer::check_hex(char c)
{
	switch (c)
	{
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
		return true;
	}
	return false;
}

TokenType Lexer::deduce_token_type(std::string& token_string)
{
	int current_index = 0;
	bool is_hex = false;
	int flag = 1;

	if (token_string.size() >= 2 && token_string[0] == '0' && token_string[1] == 'x')
	{
		is_hex = true;
		current_index += 2;
	}

	while (current_index != token_string.size())
	{
		if (!isdigit(token_string[current_index]) && !(is_hex && Lexer::check_hex(token_string[current_index])))
		{
			flag = 0;
			break;
		}
		current_index++;
	}
	if (flag == 1)
	{
		return "LITERAL";
	}

	current_index = 0;

	if (token_string[current_index] == 'r' || token_string[current_index] == 'R')
	{
		current_index++;
		if (token_string[current_index] >= '0' && token_string[current_index] <= '7')
		{
			return "REGISTER";
		}
	}

	if (directives_map.find(token_string) != directives_map.end()) return "DIRECTIVE";

	if (instructions_map.find(token_string) != instructions_map.end())
	{
		return "INSTRUCTION";
	}
	else
	{
		std::string substring = token_string.substr(0, token_string.size() - 1);
		if (instructions_map.find(substring) != instructions_map.end() && substring != "")
		{
			if (token_string[token_string.size() - 1] == 'b')
				return "INSTRUCTIONB";
			else
				return "INSTRUCTIONW";
		}
	}

	return "SYMBOL";
}

Token Lexer::get_next_multichar_token()
{
	Token token;
	token.type = "UNKNOWN";

	if (!isalnum(file_content[token_pointer]) && file_content[token_pointer] != '_'
		&& file_content[token_pointer] != '.')
	{
		return token;
	}

	int i = 0;

	while (token_pointer < file_content.size() && (isalnum(file_content[token_pointer])
		|| file_content[token_pointer] == '_' || file_content[token_pointer] == '.'))
	{
		token.token_string += file_content[token_pointer];
		token_pointer++;
	}

	TokenType type = deduce_token_type(token.token_string);

	token.type = type;
	return token;
}

Token Lexer::get_next_token()
{
	// Move token_ptr token_ptr until first occurence of character
	Token token;

	if (token_pointer >= file_content.size())
	{
		token.type = "EOF";
		token.token_string = "EOF";
		return token;
	}

	while (file_content[token_pointer] == ' ' || file_content[token_pointer] == '\t')
	{
		token_pointer++;
	}

	token.type = "UNKNOWN";

	token.token_string = file_content[token_pointer];

	switch (file_content[token_pointer])
	{
	case '&':
		token.type = "AMPERSAND";
		token_pointer++;
		break;
	case '$':
		token.type = "DOLLAR";
		token_pointer++;
		break;
	case '*':
		token.type = "ASTERISK";
		token_pointer++;
		break;
	case '[':
		token.type = "LEFT_BRACKET";
		token_pointer++;
		break;
	case ']':
		token.type = "RIGHT_BRACKET";
		token_pointer++;
		break;
	case '\0':
		token.type = "EOF";
		token_pointer++;
		break;
	case '\n':
		token.type = "EOLN";
		token_pointer++;
		break;
	case ';':
		token.type = "COMMENT";
		while (file_content[token_pointer] != '\n' && token_pointer < file_content.size())
			token_pointer++;
		//token_pointer--; // catch the last token
		break;
	case '+':
		token.type = "PLUS";
		token_pointer++;
		break;
	case '-':
		token.type = "MINUS";
		token_pointer++;
		break;
	case ':':
		token.type = "COLON";
		token_pointer++;
		break;
	case ',':
		token.type = "COMMA";
		token_pointer++;
		break;
	default:
		token = get_next_multichar_token();
		break;
	}
	if (token.token_string == "PC" || token.token_string == "pc" || token.token_string == "SP"
		|| token.token_string == "sp" || token.token_string == "PSW" || token.token_string == "psw")
	{
		token.type = "REGISTER";
	}
	return token;
}

int Lexer::get_lines_number(std::list<Token>& list)
{
	int lines = 1;
	std::list<Token>::iterator it;
	for (it = list.begin(); it != list.end(); it++)
	{
		if (it->type == "EOLN")
			lines++;
	}
	return lines;
}
int Lexer::get_line_number(std::list<Token>& list, std::list<Token>::iterator it)
{
	int lines = 1;
	std::list<Token>::iterator temp;
	for (temp = list.begin(); temp != it; temp++)
	{
		if (temp->type == "EOLN") lines++;
	}
	return lines;
}

std::list<Token> Lexer::get_token_list()
{
	std::list<Token> list;

	while (1)
	{
		Token token = get_next_token();

		if (token.type != "UNKNOWN")
		{
			if (token.type != "COMMENT") list.push_back(token);

			if (token.type == "SYMBOL" && isdigit(token.token_string[0]))
			{
				Error::symbol_start(token.token_string, Lexer::get_lines_number(list));
			}

			if (token.type == "EOF")
			{
				break;
			}
		}
		else
		{
			int line_number = get_lines_number(list);

			Error::unknown_token_type(line_number);
		}
	}
	return list;
}
