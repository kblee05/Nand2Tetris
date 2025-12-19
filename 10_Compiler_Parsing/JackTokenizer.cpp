#include "JackTokenizer.h"

void JackTokenizer::skip_whitespace(){
    static const std::string WHITE_SPACE = " \n\r\t\f\v";
    while(current_index < raw_code.length() && WHITE_SPACE.find(raw_code[current_index]) != std::string::npos)
        current_index++;
}

bool JackTokenizer::is_symbol(){
    static const std::string SYMBOLS = "{}()[].,;+-*/&|<>=~";
    return SYMBOLS.find(raw_code[current_index]) != std::string::npos;
}

bool JackTokenizer::is_integer(){
    return '0' <= raw_code[current_index] && '9' >= raw_code[current_index];
}

// ===============================public===============================

JackTokenizer::JackTokenizer(std::string filename) : current_index(0) {
    input.open(filename);
    std::stringstream buffer;
    buffer << input.rdbuf();
    std::string raw_code = buffer.str();
}

bool JackTokenizer::advance(){
    if(current_index > raw_code.length())
        return false;

    skip_whitespace();

    current_token = "";

    if(raw_code[current_index] == '/'){
        if(raw_code[current_index + 1] == '*'){
            size_t end_pos = raw_code.find("*/", current_index + 2);

            if(end_pos == std::string::npos)
                std::cerr << "Comment block unclosed\n";

            current_index = end_pos + 2;
        }
        else if(raw_code[current_index + 1] == '/'){
            size_t end_pos = raw_code.find('\n', current_index + 2);
            current_index = end_pos + 1;
        }
        else{ // Divide operator
            current_token = raw_code[current_index++];
            token_type = SYMBOL;
        }
    }
    else if(is_symbol()){
        current_token = raw_code[current_index++];
        token_type = SYMBOL;
        return true;
    }
    else if(is_integer()){
        size_t end_pos = raw_code.find_first_not_of("0123456789", current_index + 1);
        current_token = raw_code.substr(current_index, end_pos - current_index);
        current_index = end_pos;
        token_type = INT_CONST;
    }
    else if(raw_code[current_index] == '\"'){
        size_t end_pos = raw_code.find('\"', current_index + 1);
        current_token = raw_code.substr(current_index, end_pos - current_index + 1);
        current_index = end_pos + 1;
        token_type = STRING_CONST;
    }
    else{ // keyword or identifier
        size_t end_pos = raw_code.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");
        current_token = raw_code.substr(current_index, end_pos - current_index);
        current_index = end_pos;

        if(KEYWORDS.count(current_token))
            token_type = KEYWORD;
        else
            token_type = IDENTIFIER;
    }

    return true;
}

char JackTokenizer::get_symbol(){
    if(token_type == SYMBOL)
        return current_token[0];
    std::cerr << "Current token is not a symbol\n";
    exit(1);
    return '\0';
}

std::string JackTokenizer::get_identifier(){
    if(token_type == IDENTIFIER)
        return current_token;
    std::cerr << "Current token is not an identifier\n";
    exit(1);
    return "";
}

int JackTokenizer::get_int_val(){
    if(token_type == INT_CONST)
        return std::stoi(current_token);
    std::cerr << "Current token is not an integer constant\n";
    exit(1);
    return NULL;
}

std::string JackTokenizer::get_string_val(){
    if(token_type == STRING_CONST)
        return current_token.substr(1, current_token.length() - 2);
    std::cerr << "Current token is not a string constant\n";
    exit(1);
    return "";
}