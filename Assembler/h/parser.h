#ifndef _PARSER_H
#define _PARSER_H

#include <list>

#ifndef _STRING_H
#define _STRING_H

#include <string>

#endif

typedef enum op_type
{
    NO_TYPE,
    IMMED,
    IMMED_SYM_VALUE,
    REGDIR,
    REGINDDISP_IMMED,
    REGINDDISP_SYM_VALUE,
    PCREL,
    ABS,
    MEMDIR
} OperandType;

class Register
{
    friend class Operand;
    friend class Instruction;
    friend class Directive;
    friend class Line;
    friend class Parser;

private:
    int number;
    bool low = false;
    bool high = false;
};

class Operand
{
    friend class Line;
    friend class Parser;

private:
    OperandType type;
    bool is_byte;
    bool is_word;
    Register reg;
    std::string symbol;
    int literal;
};

class Line
{
    friend class Parser;

private:
    std::string label;
    bool is_directive;
    bool is_instruction;
    std::string directive;
    std::string instruction;
    std::list<Operand> operands;
};

class Parser
{
private:
    std::list<Token>::iterator token_iterator;
    Line* line_pointer;
    std::list<Token> token_list;
    int read_line();
    void read_label();
    void read_command();
    void read_operand_list();
    int read_dec_literal(int sign);
    int read_eoln();

public:
    std::list<Line> parse_token_list();
    Parser(std::list<Token>& token_list)
    {
        this->token_list = token_list;
    }
};

#endif