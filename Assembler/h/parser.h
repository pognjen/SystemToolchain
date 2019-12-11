#ifndef _PARSER_H
#define _PARSER_H

#infdef _LIST_H
#define _LIST_H
#include <list>
#endif

#ifndef _STRING_H
#define _STRING_H

#include <string>

#endif

typedef enum op_type
{
    NO_TYPE,
    IMMED, //5
    IMMED_SYM_VALUE, //&symbol
    REGDIR, //r0
    REGINDDISP_IMMED, //r0[5]
    REGINDDISP_SYM_VALUE, //r0[symb]
    PCREL, //$symbol
    ABS, //symbol
    MEMDIR //*5
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

public:
    friend std::ostream& operator<<(std::ostream& os, const Register& r) // for testing
    {
        os << "R" << r.number;
        if (r.low)
            return os << "L";
        else if (r.high)
            return os << "H";
        else
            return os;
    }
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

public:
    friend std::ostream& operator<<(std::ostream& os, const Operand& operand)
    {
        switch (operand.type)
        {
            case IMMED:
                return os << operand.literal;
            case IMMED_SYM_VALUE:
                return os << operand.symbol;
            case REGDIR:
                return os << operand.reg;
            case REGINDDISP_IMMED:
                return os << operand.reg << "[" << operand.literal << "]";
            case REGINDDISP_SYM_VALUE:
                return os << operand.reg << "[" << operand.symbol << "]";
            case PCREL:
                return os << "$" << operand.symbol;
            case ABS:
                return os << operand.symbol;
            case MEMDIR:
                return os << "*" << operand.literal;
        }
    }
};
class Line
{
    friend class Parser;

private:
    std::string label = "";
    bool is_directive = false;
    bool is_instruction = false;
	static int line_number;
	int src_line;
    std::string directive;
    std::string instruction;
    std::list<Operand> operands;
public:
    friend std::ostream& operator<<(std::ostream& os, const Line& line)
    {
        if (line.label != "")
            os << line.label << ": ";
        if (line.is_directive)
            os << line.directive << " ";
        else if (line.is_instruction)
            os << line.instruction << " ";

        for (auto& iter : line.operands)
        {
            os << iter << ',';
        }
		os<<": "<<line.src_line;
        return os;
    }
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