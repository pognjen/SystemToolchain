#include "error.h"
#include "lexer.h"
#include "parser.h"
void Parser::read_operand_list()
{
	//read first operand
		
}
void Parser::read_command()
{
	if (token_iterator != token_list.end() && token_iterator->type == "INSTRUCTION")
	{
		line_pointer->is_instruction = true;
		line_pointer->is_directive = false;
		
		line->instruction = token_iterator->token_string;
		token_iterator++;
	}
	else
		if(token_iterator != token_list.end() && token_iterator->type == "DIRECTIVE")
		{
			ine_pointer->is_instruction = false;
			line_pointer->is_directive = true;
			
			line->directive = token_iterator->token_string;
			token_iterator++;
		}
}
void Parser::read_label()
{
	if (token_iterator->type == "SYMBOL")
	{
		line_pointer->label = token_iterator->token_string;
		token_iterator++;
		
		if (token_iterator->type == "COLON")
		{
			while (token_iterator->type == "EOLN")
			{
				token_iterator++;
			}
		}
		else
			Error::colon_expected(Lexer::get_lines_number(token_list),token_iterator->token_string);
	}
}

int Parser::read_line()
{
	line_pointer = new Line();
	
	read_label();
	
	read_command();
	
	read_operand_list();
}
std::list<Line> Parser::parse_token_list()
{
	token_iterator = token_list.begin();
	
	std::list<Line> line_list;
	
	while (1)
	{
		int end = read_line();
		
		if (end != 1)
		{
			
		}
		else
			break;
	}
	
	return line_list;
}