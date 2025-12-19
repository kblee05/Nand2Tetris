#include "JackTokenizer.h"

// ===============================private==============================

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

void JackTokenizer::set_keyword_type(){
    if(current_token == "class") keyword_type = CLASS;
    else if(current_token == "method") keyword_type = METHOD;
    else if(current_token == "function") keyword_type = FUNCTION;
    else if(current_token == "constructor") keyword_type = CONSTRUCTOR;
    else if(current_token == "int") keyword_type = INT;
    else if(current_token == "boolean") keyword_type = BOOLEAN;
    else if(current_token == "char") keyword_type = CHAR;
    else if(current_token == "void") keyword_type = VOID;
    else if(current_token == "var") keyword_type = VAR;
    else if(current_token == "static") keyword_type = STATIC;
    else if(current_token == "field") keyword_type = FIELD;
    else if(current_token == "let") keyword_type = LET;
    else if(current_token == "do") keyword_type = DO;
    else if(current_token == "if") keyword_type = IF;
    else if(current_token == "else") keyword_type = ELSE;
    else if(current_token == "while") keyword_type = WHILE;
    else if(current_token == "return") keyword_type = RETURN;
    else if(current_token == "method") keyword_type = METHOD;
    else if(current_token == "true") keyword_type = TRUE;
    else if(current_token == "false") keyword_type = FALSE;
    else if(current_token == "null") keyword_type = _NULL;
    else if(current_token == "this") keyword_type = THIS;
}

// ===============================public===============================

JackTokenizer::JackTokenizer(std::ifstream& ifstream) : input(ifstream), current_index(0) {
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

        if(KEYWORDS.count(current_token)){
            token_type = KEYWORD;
            set_keyword_type();
        }
        else
            token_type = IDENTIFIER;
    }

    return true;
}

KeywordType JackTokenizer::get_keyword_type(){
    if(token_type != KEYWORD){
        throw std::runtime_error("Current token type is not KEYWORD]n");
    }

    return keyword_type;
}

char JackTokenizer::get_symbol(){
    if(token_type == SYMBOL)
        return current_token[0];
    
    throw std::runtime_error("Current token " + current_token + " is not a symbol\n");
}

std::string JackTokenizer::get_identifier(){
    if(token_type == IDENTIFIER)
        return current_token;
    
    throw std::runtime_error("Current token " + current_token + " is not an identifier\n");
}

int JackTokenizer::get_int_val(){
    if(token_type == INT_CONST)
        return std::stoi(current_token);
    
    throw std::runtime_error("Current token " + current_token + " is not an integer\n");
}

std::string JackTokenizer::get_string_val(){
    if(token_type == STRING_CONST)
        return current_token.substr(1, current_token.length() - 2);
    
    throw std::runtime_error("Current token " + current_token + " is not a string\n");
}