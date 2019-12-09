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
    static void colon_expected(std::string symbol)
    {
        std::cout << "Error: colon exprected after " << symbol << '\n';
        exit(-1);
    }
    static void literal_expected()
    {
        std::cout << "Error: literal expected\n";
        exit(-1);
    }
    static void wrong_literal_width()
    {
        std::cout << "Error: wrong witdh of literal\n";
        exit(-1);
    }

    static void symbol_expected_after_ampersand()
    {
        std::cout << "Error: symbol expected after &\n";
        exit(-1);
    }
    static void symbol_or_literal_expected()
    {
        std::cout << "Error: symbol or literal expected\n";
        exit(-1);
    }
    static void right_bracket_expected()
    {
        std::cout << "Error: [ expected\n";
        exit(-1);
    }
    static void symbol_expected_after_dollar()
    {
        std::cout << "Error: symbol expected after $\n";
        exit(-1);
    }
    static void invalid_statement()
    {
        std::cout << "Error: invalid_statement\n";
        exit(-1);
    }
};

#endif
