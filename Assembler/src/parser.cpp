#include "error.h"
#include "parser.h"
#include <cstdint>
#include <unordered_map>
#include <unordered_set>

extern std::unordered_map<std::string, int>  instructions_map;
extern std::unordered_set<std::string>  directives_map;

int Line::line_number = 0;

int Parser::read_dec_literal(int sign)
{
	std::string literal = token_iterator->token_string;
	int i = 0;
	bool is_hex = false;
	if (literal.size() >= 3 && literal[0] == '0' && literal[1] == 'x')
	{
		i = i + 2;
		is_hex = true;
	}
   /* for (; i < literal.size(); i++)
    {
        if (!isdigit(literal[i]) && (!is_hex))
        {
           Error::literal_expected(Lexer::get_line_number(token_list,token_iterator));
        }
    }*/
    int retVal ;
	
	if (is_hex) retVal = stoi(literal,0,16);
	else
		retVal = stoi(literal,0,10);
    
	retVal = retVal * sign;

    if (retVal > INT16_MAX || retVal < INT16_MIN)
    {
        Error::wrong_literal_width(Lexer::get_line_number(token_list,token_iterator));
    }

    token_iterator++;

    return retVal;
}
int Parser::read_eoln()
{
    if (token_iterator->type == "EOLN")
    {
        token_iterator++;
		Line::line_number++;
        return 0;
    }
    else if (token_iterator->type == "EOF")
    {
        token_iterator++;
		Line::line_number++;
        return 1;
    }
    else
    {
        Error::invalid_statement(Lexer::get_line_number(token_list,token_iterator));
    }
}
void Parser::read_operand_list()
{
    std::string type = token_iterator->type;
    if (type == "PLUS" || type == "MINUS" || type == "LITERAL" || type == "AMPERSAND"
        || type == "DOLLAR" || type == "ASTERISK" || type == "SYMBOL" || type == "REGISTER" || type == "DIRECTIVE")
    {
        while (1)
        {
            Operand* operand = new Operand();
            int sign = 1;
            if (type == "PLUS" || type == "MINUS" || type == "LITERAL")
            {
                if (type == "MINUS")
                {
                    sign = -1;
                }

                if (type == "PLUS" || type == "MINUS")
                {
                    token_iterator++;
					type = token_iterator->type;
                }

				if (type != "LITERAL")
				{
					Error::literal_expected(Lexer::get_line_number(token_list,token_iterator));
				}
                int retVal = read_dec_literal(sign);

				if (retVal <= INT8_MAX && retVal >= INT8_MIN)
				{
					operand->is_word = false;
					operand->is_byte = true;
				}
				else
				{
					operand->is_word = true;
					operand->is_byte = false;
				}
                operand->type = IMMED;
                operand->literal = retVal;

                line_pointer->operands.push_back(*operand);

                // type = global_token_ptr->token.token_type;
                type = token_iterator->type;
            }
            else if (type == "AMPERSAND")
            {
                token_iterator++;
                type = token_iterator->type;

                if (type == "SYMBOL")
                {
                    operand->is_word = true;
                    operand->is_byte = false;
                    operand->type = IMMED_SYM_VALUE;
                    operand->symbol = token_iterator->token_string;

                    line_pointer->operands.push_back(*operand);

                    token_iterator++;

                    type = token_iterator->type;
                }
                else
                    Error::symbol_expected_after_ampersand(Lexer::get_line_number(token_list,token_iterator));
            }
            else if (type == "REGISTER")
            {
                operand->is_word = true;
                operand->is_byte = false;
                operand->type = REGDIR;
				
				if (token_iterator->token_string != "psw" && token_iterator->token_string != "sp" && token_iterator->token_string != "pc")
				{
					operand->reg.number = token_iterator->token_string[1] - '0';
				}
				else
					if (token_iterator->token_string == "psw" || token_iterator->token_string == "PSW") operand->reg.number = 15;
					else
						if (token_iterator->token_string == "pc" || token_iterator->token_string == "PC") operand->reg.number = 7;
						else
							if (token_iterator->token_string == "sp" || token_iterator->token_string == "SP") operand->reg.number = 6;

                if (token_iterator->token_string.size() == 3)
                {
                    if (token_iterator->token_string[2] == 'h'
                        || token_iterator->token_string[2] == 'H')
                    {
                        operand->reg.high = true;
                    }
                    else
                    {
                        operand->reg.low = true;
                    }
					operand->is_word = false;
					operand->is_byte = true;
                }
                token_iterator++;
                type = token_iterator->type;

                if (type == "LEFT_BRACKET")
                {
                    token_iterator++;
                    type = token_iterator->type;

                    if (type == "PLUS" || type == "MINUS" || type == "LITERAL")
                    {
                        int sign = 1;
                        if (type == "MINUS")
                        {
                            sign = -1;
                            token_iterator++;
                            type = token_iterator->type;
                        }
                        else if (type == "PLUS")
                        {
                            sign = +1;
                            token_iterator++;
                            type = token_iterator->type;
                        }

                        if (type == "LITERAL")
                        {
                            operand->type = REGINDDISP_IMMED;
                            operand->literal = read_dec_literal(sign);
							operand->is_word = true;
							operand->is_byte = false;
							
                            // global_token_ptr = global_token_ptr->next;
                            type = token_iterator->type;
                        }
                        else
                            Error::literal_expected(Lexer::get_line_number(token_list,token_iterator));
                    }
                    else if (type == "SYMBOL")
                    {
                        operand->type = REGINDDISP_SYM_VALUE;
                        operand->symbol = token_iterator->token_string;

						operand->is_word = true;
						operand->is_byte = false;

                        token_iterator++;
                        type = token_iterator->type;
                    }
                    else
                        Error::symbol_or_literal_expected(Lexer::get_line_number(token_list,token_iterator));

                    if (type == "RIGHT_BRACKET")
                    {
                        token_iterator++;
                        type = token_iterator->type;
                    }
                    else
                        Error::right_bracket_expected(Lexer::get_line_number(token_list,token_iterator));
                }
                line_pointer->operands.push_back(*operand);
            }
            else if (type == "DOLLAR")
            {
                token_iterator++;
                type = token_iterator->type;

                if (type == "SYMBOL")
                {
                    operand->is_word = true;
                    operand->is_byte = false;
                    operand->type = PCREL;
                    operand->symbol = token_iterator->token_string;

                    line_pointer->operands.push_back(*operand);

                    token_iterator++;
                    type = token_iterator->type;
                }
                else
                {
                    Error::symbol_expected_after_dollar(Lexer::get_line_number(token_list,token_iterator));
                }
            }
            else if (type == "SYMBOL" || type == "DIRECTIVE" )
            {
                operand->is_word = true;
                operand->is_byte = false;
                operand->type = ABS;
                operand->symbol = token_iterator->token_string;
                line_pointer->operands.push_back(*operand);

                token_iterator++;
                type = token_iterator->type;
            }
            else if (type == "ASTERISK")
            {
                token_iterator++;
                type = token_iterator->type;

                if (type == "LITERAL" || type == "MINUS" || type == "PLUS")
                {
                    int sign = 1;
                    if (type == "MINUS")
                    {
                        sign = -1;
                        token_iterator++;
                        type = token_iterator->type;
                    }
                    else if (type == "PLUS")
                    {
                        sign = +1;
                        token_iterator++;
                        type = token_iterator->type;
                    }
                    if (type == "LITERAL")
                    {
                        operand->is_word = true;
                        operand->is_byte = false;
                        operand->type = MEMDIR;
                        operand->literal = read_dec_literal(sign);

                        line_pointer->operands.push_back(*operand);

                        // global_token_ptr = global_token_ptr->next;
                        type = token_iterator->type;
                    }
                    else
                        Error::literal_expected(Lexer::get_line_number(token_list,token_iterator));
                }
                else
                    Error::literal_expected(Lexer::get_line_number(token_list,token_iterator));
            }

            if (type == "COMMA")
            {
                token_iterator++;
                type = token_iterator->type;
            }
            else
                break;
        }
    }
}
void Parser::read_command()
{
    if (token_iterator != token_list.end() && (token_iterator->type == "INSTRUCTION" || token_iterator->type == "INSTRUCTIONB" || token_iterator->type == "INSTRUCTIONW"))
    {
        line_pointer->is_instruction = true;
        line_pointer->is_directive = false;

        line_pointer->instruction = token_iterator->token_string;
		if (token_iterator->type == "INSTRUCTIONB")
		{
			line_pointer->operands_byte = true;
			line_pointer->operands_word = false;
			line_pointer->instruction = token_iterator->token_string.substr(0, token_iterator->token_string.size() - 1);
		}
		else
			if (token_iterator->type == "INSTRUCTIONW")
			{
				{
					line_pointer->operands_byte = false;
					line_pointer->operands_word = true;
					line_pointer->instruction = token_iterator->token_string.substr(0, token_iterator->token_string.size() - 1);
				}
			}
			else
				line_pointer->operands_word = false, line_pointer->operands_byte = false;
        token_iterator++;
    }
    else if (token_iterator != token_list.end() && token_iterator->type == "DIRECTIVE")
    {
        line_pointer->is_instruction = false;
        line_pointer->is_directive = true;

        line_pointer->directive = token_iterator->token_string;
		line_pointer->operands_word = true, line_pointer->operands_byte = false;
        token_iterator++;
    }
}
void Parser::read_label()
{
    if (token_iterator->type == "SYMBOL")
    {
        line_pointer->label = token_iterator->token_string;
		line_pointer->label_src_line = Line::line_number;
        token_iterator++;

        if (token_iterator->type == "COLON")
        {
			token_iterator++;
            while (token_iterator->type == "EOLN")
            {
				Line::line_number++;
                token_iterator++;
            }
        }
        else
            Error::colon_expected(token_iterator->token_string,Lexer::get_line_number(token_list,token_iterator));
    }
	else
		line_pointer->label = "";
}

int Parser::read_line()
{
    line_pointer = new Line();

    read_label();

    read_command();

    read_operand_list();

    return read_eoln();
}
std::list<Line> Parser::parse_token_list()
{
    token_iterator = token_list.begin();

    std::list<Line> line_list;

    while (1)
    {
        int end = read_line();
		
		line_pointer->src_line = Line::line_number;
        line_list.push_back(*line_pointer);

		if (end || line_pointer->directive == ".end")
		{
			break;
		}
    }
    return line_list;
}