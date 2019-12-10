#include "error.h"
#include "lexer.h"
#include "parser.h"
#include <cstdint>

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
    for (; i < literal.size(); i++)
    {
        if (literal[i] < '0' || literal[i] > '9')
        {
           Error::literal_expected();
        }
    }
    int retVal ;
	
	if (is_hex) retVal = stoi(literal,0,16);
	else
		retVal = stoi(literal,0,10);
    
	retVal = retVal * sign;

    if (retVal > INT16_MAX || retVal < INT16_MIN)
    {
        Error::wrong_literal_width();
    }

    token_iterator++;

    return retVal;
}
int Parser::read_eoln()
{
    if (token_iterator->type == "EOLN")
    {
        token_iterator++;
        return 0;
    }
    else if (token_iterator->type == "EOF")
    {
        token_iterator++;
        return 1;
    }
    else
    {
        Error::invalid_statement();
    }
}
void Parser::read_operand_list()
{
    std::string type = token_iterator->type;
    if (type == "PLUS" || type == "MINUS" || type == "LITERAL" || type == "AMPERSAND"
        || type == "DOLLAR" || type == "ASTERISK" || type == "SYMBOL" || type == "REGISTER")
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

                if (type != "LITERAL")
                {
                    token_iterator++;
                }

                int retVal = read_dec_literal(sign);

                operand->is_word = true;
                operand->is_byte = false;
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
                    Error::symbol_expected_after_ampersand();
            }
            else if (type == "REGISTER")
            {
                operand->is_word = true;
                operand->is_byte = false;
                operand->type = REGDIR;
                operand->reg.number = token_iterator->token_string[1] - '0';

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

                            // global_token_ptr = global_token_ptr->next;
                            type = token_iterator->type;
                        }
                        else
                            Error::literal_expected();
                    }
                    else if (type == "SYMBOL")
                    {
                        operand->type = REGINDDISP_SYM_VALUE;
                        operand->symbol = token_iterator->token_string;

                        token_iterator++;
                        type = token_iterator->type;
                    }
                    else
                        Error::symbol_or_literal_expected();

                    if (type == "RIGHT_BRACKET")
                    {
                        token_iterator++;
                        type = token_iterator->type;
                    }
                    else
                        Error::right_bracket_expected();
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
                    Error::symbol_expected_after_dollar();
                }
            }
            else if (type == "SYMBOL")
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
                        Error::literal_expected();
                }
                else
                    Error::literal_expected();
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
    if (token_iterator != token_list.end() && token_iterator->type == "INSTRUCTION")
    {
        line_pointer->is_instruction = true;
        line_pointer->is_directive = false;

        line_pointer->instruction = token_iterator->token_string;
        token_iterator++;
    }
    else if (token_iterator != token_list.end() && token_iterator->type == "DIRECTIVE")
    {
        line_pointer->is_instruction = false;
        line_pointer->is_directive = true;

        line_pointer->directive = token_iterator->token_string;
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
			token_iterator++;
            while (token_iterator->type == "EOLN")
            {
                token_iterator++;
            }
        }
        else
            Error::colon_expected(token_iterator->token_string);
    }
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

        if (end != 1)
        {
            line_list.push_back(*line_pointer);
        }
        else
            break;
    }
    return line_list;
}