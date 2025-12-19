#ifndef JACKTOKENIZER_H
#define JACKTOKENIZER_H

#include "Types.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>

class JackTokenizer{
    std::string current_token;
    std::string raw_code;
    int current_index;
    std::ifstream input;
    void skip_whitespace();
    bool is_symbol();
    bool is_integer();
public:
    TokenType token_type;
    JackTokenizer(std::string filename);
    bool advance();
    char get_symbol();
    std::string get_identifier();
    int get_int_val();
    std::string get_string_val();
};

#endif