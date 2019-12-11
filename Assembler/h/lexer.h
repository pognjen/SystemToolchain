#ifndef _LEXER_H
#define _LEXER_H

#include <string>

#ifndef _LIST_H
#define _LIST_H

#include <list>

#endif

#include <unordered_map>

#ifndef _FSTREAM_H
#define _FSTREAM_H

#include <fstream>
#endif

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
    std::unordered_map<std::string, std::string> map;
    std::ifstream instructions;
    std::ifstream directives;

    unsigned int token_pointer;
    Token get_next_token();
    Token get_next_multichar_token();
    TokenType deduce_token_type(std::string& token_string);

public:
    static int get_lines_number(std::list<Token>& list);
	static int get_line_number(std::list<Token>& list,std::list<Token>::iterator it);
    std::unordered_map<std::string, std::string>& get_map()
    {
        return map;
    }
    Lexer(std::string& file_content)
    {
        this->file_content = file_content;
        token_pointer = 0;

        instructions.open("instructions.txt");
        directives.open("directives.txt");

        while (!instructions.eof())
        {
            std::string s;
            std::getline(instructions, s);
            map.insert({ s, "INSTRUCTION" });
        }

        while (!directives.eof())
        {
            std::string s;
            std::getline(directives, s);
            map.insert({ s, "DIRECTIVE" }); // directive
        }

        /*for (auto& it: map)
        {
            std::cout << it.first<<'\n';
        }*/
    }
    std::list<Token> get_token_list();
};

#endif
