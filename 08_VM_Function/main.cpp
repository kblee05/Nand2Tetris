#include "Parser.h"
#include "CodeWriter.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]){
    if(argc < 3){
        std::cerr << "Usage: ./VMT <asmname.asm> <filename1.vm> <filename2.vm> ...\n";
        return 1;
    }

    CodeWriter code_writer(argv[1]);

    if(argc > 3)
        code_writer.bootstrap();


    for(size_t i=2; i<argc; i++){
        code_writer.set_file_name(argv[i]);
        Parser parser(argv[i]); // vm files

        while(parser.advance()){
            CommandType command = parser.get_command_type();
            switch (command)
            {
            case C_PUSH:
                code_writer.write_pushpop(command, parser.arg1, parser.arg2);
                break;
            case C_POP:
                code_writer.write_pushpop(command, parser.arg1, parser.arg2);
                break;
            case C_ARITHMETIC:
                code_writer.write_arithmetic(parser.arg1);
                break;
            case C_LABEL:
                code_writer.write_label(parser.arg1);
                break;
            case C_GOTO:
                code_writer.write_goto(parser.arg1);
                break;
            case C_IF:
                code_writer.write_if(parser.arg1);
                break;
            case C_FUNCTION:
                code_writer.write_function(parser.arg1, parser.arg2);
                break;
            case C_RETURN:
                code_writer.write_return();
                break;
            case C_CALL:
                code_writer.write_call(parser.arg1, parser.arg2);
                break;
            }
        }
    }

    return 0;
}