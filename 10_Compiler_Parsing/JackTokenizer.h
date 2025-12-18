#ifndef JACKTOKENIZER_H
#define JACKTOKENIZER_H

#include "Types.h"
#include <string>
#include <fstream>
#include <set>

class JackTokenizer{
    std::string current_token;
    std::ifstream input;
    void skip_whitespace();
    bool is_symbol(char c);
    bool is_integer(char c);
public:
    JackTokenizer(std::string filename);
    bool advance();
    TokenType get_token_type();
    char get_symbol();
    std::string get_identifier();
    int get_int_val();
    std::string get_string_val();
};

#endif