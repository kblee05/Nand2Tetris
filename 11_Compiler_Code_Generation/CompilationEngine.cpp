#include "CompilationEngine.h"

// ===============================private==============================

void CompilationEngine::eat(std::string expected_token){
    if(tokenizer.current_token == expected_token){
        tokenizer.advance();
        return;
    }
    throw std::runtime_error("Current token: " + tokenizer.current_token + " does not match " + expected_token + "\n");
}

void CompilationEngine::eat_identifier(){
    if(tokenizer.token_type == IDENTIFIER){
        tokenizer.advance();
        return;
    }
    throw std::runtime_error("Current token: " + tokenizer.current_token + " is not an IDENTIFIER\n");
}

void CompilationEngine::print_xml(std::string tag, std::string body){
    for(int i=0; i<identation_level; i++)
        output << "  ";

    if(body == "<")
        output << "<" << tag << "> &lt; </" << tag << ">\n";
    else if(body == ">")
        output << "<" << tag << "> &gt; </" << tag << ">\n";
    else if(body == "\"")
        output << "<" << tag << "> &quot; </" << tag << ">\n";
    else if(body == "&")
        output << "<" << tag << "> &amp; </" << tag << ">\n";
    else
        output << "<" << tag << "> " << body << " </" << tag << ">\n"; 
}

void CompilationEngine::print_structure(std::string tag, bool is_start){
    if(is_start){ // start of identation
        for(int i=0; i<identation_level; i++)
            output << "  ";
        output << "<" << tag << ">\n";
        identation_level++;
    }
    else{ // end of identation
        identation_level--;
        for(int i=0; i<identation_level; i++)
            output << "  ";
        output << "</" << tag << ">\n";
    }
}

void CompilationEngine::compile_type(){
    if(tokenizer.current_token == "int" ||
       tokenizer.current_token == "char" ||
       tokenizer.current_token == "boolean")
    {
        print_xml("keyword", tokenizer.current_token);
        tokenizer.advance();
    }
    else{
        print_xml("identifier", tokenizer.current_token);
        eat_identifier();
    }
}

VMWriter::Segment kind_to_segment(SymbolTable::Kind kind){
    switch (kind){
    case SymbolTable::Kind::VAR:
        return VMWriter::Segment::LOCAL;
    case SymbolTable::Kind::FIELD:
        return VMWriter::Segment::THIS;
    case SymbolTable::Kind::ARG:
        return VMWriter::Segment::ARG;
    case SymbolTable::Kind::STATIC:
        return VMWriter::Segment::STATIC;
    default:
        throw std::runtime_error("Unkown kind of mapping ( SymbolTable kind to VMWriter)");
    }
}

// ===============================public===============================

CompilationEngine::CompilationEngine(std::ofstream& ofstream, JackTokenizer& tokenizer) : output(ofstream), identation_level(0), tokenizer(tokenizer) {
    
}

void CompilationEngine::compile_class(){
    print_structure("class", 1);

    print_xml("keyword", "class");
    eat("class");

    print_xml("identifier", tokenizer.current_token);
    tokenizer.advance();

    print_xml("symbol", "{");
    eat("{");

    while(tokenizer.current_token == "field" || tokenizer.current_token == "static")
        compile_class_var_dec();
    
    while(tokenizer.current_token == "constructor" ||
          tokenizer.current_token == "function" ||
          tokenizer.current_token == "method")
        compile_subroutine();

    print_xml("symbol", "}");
    eat("}");

    print_structure("class", 0);
}

void CompilationEngine::compile_class_var_dec(){
    print_structure("classVarDec", true);

    print_xml("keyword", tokenizer.current_token);
    tokenizer.advance();

    compile_type();

    print_xml("identifier", tokenizer.current_token);
    eat_identifier();

    while(tokenizer.current_token == ","){
        print_xml("symbol", ",");
        eat(",");

        print_xml("identifier", tokenizer.current_token);
        eat_identifier();
    }

    print_xml("symbol", ";");
    eat(";");

    print_structure("classVarDec", false);
}

void CompilationEngine::compile_subroutine(){
    print_structure("subroutineDec", 1);

    print_xml("keyword", tokenizer.current_token); // constructor function method
    tokenizer.advance();

    if(tokenizer.current_token == "void"){
        print_xml("keyword", "void");
        tokenizer.advance();
    }
    else
        compile_type();

    print_xml("identifier", tokenizer.current_token);
    eat_identifier();

    print_xml("symbol", "(");
    eat("(");

    compile_parameter_list();

    print_xml("symbol", ")");
    eat(")");

    compile_subroutine_body();

    print_structure("subroutineDec", 0);
}

void CompilationEngine::compile_parameter_list(){
    print_structure("parameterList", 1);

    if(tokenizer.current_token != ")"){
        compile_type();

        print_xml("identifier", tokenizer.current_token);
        eat_identifier();
    }

    while(tokenizer.current_token == ","){
        print_xml("symbol", ",");
        eat(",");

        compile_type();

        print_xml("identifier", tokenizer.current_token);
        eat_identifier();
    }

    print_structure("parameterList", 0);
}

void CompilationEngine::compile_subroutine_body(){
    print_structure("subroutineBody", 1);

    print_xml("symbol", "{");
    eat("{");

    while(tokenizer.current_token == "var")
        compile_var_dec();

    compile_statements();

    print_xml("symbol", "}");
    eat("}");

    print_structure("subroutineBody", 0);
}

void CompilationEngine::compile_var_dec(){
    print_structure("varDec", 1);

    print_xml("keyword", "var");
    eat("var");

    compile_type();

    print_xml("identifier", tokenizer.current_token);
    eat_identifier();

    while(tokenizer.current_token == ","){
        print_xml("symbol", ",");
        eat(",");

        print_xml("identifier", tokenizer.current_token);
        eat_identifier();
    }

    print_xml("symbol", ";");
    eat(";");

    print_structure("varDec", 0);
}

void CompilationEngine::compile_statements(){
    print_structure("statements", 1);

    while(tokenizer.token_type == KEYWORD){
        if(tokenizer.current_token == "let")
            compile_let();
        else if(tokenizer.current_token == "if")
            compile_if();
        else if(tokenizer.current_token == "while")
            compile_while();
        else if(tokenizer.current_token == "do")
            compile_do();
        else if(tokenizer.current_token == "return")
            compile_return();
        else
            return;
    }

    print_structure("statements", 0);
}

void CompilationEngine::compile_let(){
    print_structure("letStatement", 1);

    print_xml("keyword", "let");
    eat("let");

    print_xml("identifier", tokenizer.current_token);
    eat_identifier();

    if(tokenizer.current_token == "["){
        print_xml("symbol", "[");
        tokenizer.advance();

        compile_expression();

        print_xml("symbol", "]");
        eat("]");
    }

    print_xml("symbol", "=");
    eat("=");

    compile_expression();

    print_xml("symbol", ";");
    eat(";");

    print_structure("letStatement", 0);
}

void CompilationEngine::compile_if(){
    print_structure("ifStatement", 1);

    print_xml("keyword", "if");
    eat("if");

    print_xml("symbol", "(");
    eat("(");

    compile_expression();

    print_xml("symbol", ")");
    eat(")");

    print_xml("symbol", "{");
    eat("{");

    compile_statements();

    print_xml("symbol", "}");
    eat("}");

    if(tokenizer.current_token == "else"){
        print_xml("keyword", "else");
        tokenizer.advance();

        print_xml("symbol", "{");
        eat("{");

        compile_statements();

        print_xml("symbol", "}");
        eat("}");
    }

    print_structure("ifStatement", 0);
}

void CompilationEngine::compile_while(){
    print_structure("whileStatement", 1);

    print_xml("keyword", "while");
    eat("while");

    print_xml("symbol", "(");
    eat("(");

    compile_expression();

    print_xml("symbol", ")");
    eat(")");

    print_xml("symbol", "{");
    eat("{");

    compile_statements();

    print_xml("symbol", "}");
    eat("}");

    print_structure("whileStatement", 0);
}

void CompilationEngine::compile_do(){
    print_structure("doStatement", 1);

    print_xml("keyword", "do");
    eat("do");

    print_xml("identifier", tokenizer.current_token);
    eat_identifier();

    if(tokenizer.current_token == "."){
        print_xml("symbol", ".");
        tokenizer.advance();

        print_xml("identifier", tokenizer.current_token);
        eat_identifier();
    }

    print_xml("symbol", "(");
    eat("(");

    compile_expression_list();

    print_xml("symbol", ")");
    eat(")");

    print_xml("symbol", ";");
    eat(";");

    print_structure("doStatement", 0);
}

void CompilationEngine::compile_return(){
    print_structure("returnStatement", 1);

    print_xml("keyword", "return");
    eat("return");

    if(tokenizer.current_token != ";")
        compile_expression();

    print_xml("symbol", ";");
    eat(";");

    print_structure("returnStatement", 0);
}

void CompilationEngine::compile_expression(){
    print_structure("expression", 1);

    compile_term();

    static const std::string OPERATORS = "+-*/&|<>=";

    while(OPERATORS.find(tokenizer.current_token) != std::string::npos){
        print_xml("symbol", tokenizer.current_token);
        tokenizer.advance();

        compile_term();
    }

    print_structure("expression", 0);
}

void CompilationEngine::compile_term(){
    print_structure("term", 1);

    switch (tokenizer.token_type)
    {
    case INT_CONST:
        print_xml("integerConstant", tokenizer.current_token);
        tokenizer.advance();
        break;
    case STRING_CONST:
        print_xml("stringConstant", tokenizer.current_token.substr(1, tokenizer.current_token.length()- 2));
        tokenizer.advance();
        break;
    case IDENTIFIER:
        print_xml("identifier", tokenizer.current_token);
        eat_identifier();

        if(tokenizer.current_token == "["){
            print_xml("symbol", "[");
            tokenizer.advance();

            compile_expression();

            print_xml("symbol", "]");
            eat("]");
        }
        else if(tokenizer.current_token == "(" || tokenizer.current_token == "."){
            if(tokenizer.current_token == "."){
                print_xml("symbol", ".");
                tokenizer.advance();

                print_xml("identifier", tokenizer.current_token);
                eat_identifier();
            }

            print_xml("symbol", "(");
            tokenizer.advance();

            compile_expression_list();

            print_xml("symbol", ")");
            eat(")");
        }
        break;
    default:
        if(tokenizer.current_token == "-" || tokenizer.current_token == "~"){ // unary operator
            print_xml("symbol", tokenizer.current_token);
            tokenizer.advance();
            compile_term();
        }
        else if(tokenizer.current_token == "("){ // expression
            print_xml("symbol", "(");
            tokenizer.advance();

            compile_expression();

            print_xml("symbol", ")");
            eat(")");
        }
        else if(tokenizer.current_token == "true" || // keyword constant: true false null this
                tokenizer.current_token == "false" ||
                tokenizer.current_token == "null" ||
                tokenizer.current_token == "this")
        {
            print_xml("keyword", tokenizer.current_token);
            tokenizer.advance();
        }
        break;
    }

    print_structure("term", 0);
}

void CompilationEngine::compile_expression_list(){
    print_structure("expressionList", 1);

    if(tokenizer.current_token != ")")
        compile_expression();

    while(tokenizer.current_token == ","){
        print_xml("symbol", ",");
        tokenizer.advance();

        compile_expression();
    }

    print_structure("expressionList", 0);
}