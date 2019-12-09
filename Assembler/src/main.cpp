#include <iostream>
#include "file_content.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"

int main(int argc, char* argv[])
{
    // parse cmd arguments

    // read content from file
    FileContent file("file.txt");

    std::string file_content = file.get_file_content();

    if (file_content == "")
    {
        Error::file_opening_error("file.txt");
    }

    // get list of tokens from that file
    Lexer lexer(file_content);
    std::list<Token> token_list = lexer.get_token_list();

    std::list<Token>::iterator it;
    for (it = token_list.begin(); it != token_list.end(); it++)
    {
        std::cout << it->token_string << " : " << it->type << '\n';
    }

    // parse token list
    // Parser parser(token_list);
    // std::list<Line> line_list = parser.parse_token_list();
}
