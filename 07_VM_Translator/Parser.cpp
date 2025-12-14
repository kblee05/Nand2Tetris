#include "Parser.h"

Parser::Parser(std::string filename) : filename(filename) {
    input.open(filename);
}

std::string Parser::clean_line(std::string line){
    size_t comment_pos = line.find("//");

    if(comment_pos != std::string::npos)
        line = line.substr(0, comment_pos);

    const std::string WHITE_SPACE = "\n\r\t\f\v";

    size_t end_pos = line.find_last_not_of(WHITE_SPACE);
    if(end_pos == std::string::npos) // empty line
        return "";
    line = line.substr(0, end_pos+1);

    size_t start_pos = line.find_first_not_of(WHITE_SPACE);
    if(start_pos != std::string::npos)
        line = line.substr(start_pos);

    return line;
}

bool Parser::advance(){
    std::string line;

    while(std::getline(input, line)){
        std::string cleaned_line = clean_line(line);

        if(cleaned_line.empty())
            continue;

        current_command = cleaned_line;
        parse(cleaned_line);
        return true;
    }
    return false;
}

CommandType Parser::get_command_type(){
    if(arg0 == "push") return C_PUSH;
    if(arg0 == "pop") return C_POP;
    if(arg0 == "label") return C_LABEL;
    if(arg0 == "goto") return C_GOTO;
    if(arg0 == "if-goto") return C_IF;
    if(arg0 == "function") return C_FUNCTION;
    if(arg0 == "return") return C_RETURN;
    if(arg0 == "call") return C_CALL;
    return C_ARITHMETIC;
}

void Parser::parse(std::string cleaned_line){
    std::stringstream ss(current_command);
    ss >> arg0;

    if(get_command_type() == C_ARITHMETIC){
        arg1 = arg0;
        return;
    }
    if(get_command_type() == C_RETURN)
        return;
    
    ss >> arg1 >> arg2;
}