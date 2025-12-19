#ifndef JACKTOKENIZER_H
#define JACKTOKENIZER_H

#include "Types.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

class JackTokenizer{
private:
    std::string raw_code;
    int current_index;
    std::ifstream& input;
    KeywordType keyword_type;
    void skip_whitespace();
    bool is_symbol();
    bool is_integer();
    void set_keyword_type();
    
public:
    std::string current_token;
    TokenType token_type;
    JackTokenizer(std::ifstream& ifstream);
    bool advance();
    KeywordType get_keyword_type();
    char get_symbol();
    std::string get_identifier();
    int get_int_val();
    std::string get_string_val();
};

#endif