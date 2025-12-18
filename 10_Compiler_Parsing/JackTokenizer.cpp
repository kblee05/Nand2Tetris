#include "JackTokenizer.h"
#include "Types.h"

void JackTokenizer::skip_whitespace(){
    
}

bool JackTokenizer::is_symbol(char c){
    static const std::string symbols = "{}()[].,;+-*/&|<>=~";
    static const std::set<char> symbol_set(symbols.begin(), symbols.end());
    return symbol_set.count(c);
}

// ===============================public===============================

JackTokenizer::JackTokenizer(std::string filename){
    input.open(filename);
}

bool JackTokenizer::advance(){
    if(input.eof())
        return false;

    skip_whitespace();

    current_token = "";
    char c = input.peek();

    if(is_symbol(c)){
        current_token = input.get();
        return true;
    }
    else if(is_integer(c)){

    }

    return true;
}

TokenType JackTokenizer::get_token_type(){

}

char JackTokenizer::get_symbol(){

}

std::string JackTokenizer::get_identifier(){

}

int JackTokenizer::get_int_val(){

}

std::string JackTokenizer::get_string_val(){

}