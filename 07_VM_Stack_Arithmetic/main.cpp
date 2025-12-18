#include "Parser.h"
#include "CodeWriter.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]){
    if(argc < 2){
        std::cerr << "Usage: ./VMT <filename1.vm> <filename2.vm> ...\n";
        return 1;
    }

    for(size_t i=1; i<argc; i++){
        std::string filename = argv[i];

        Parser parser(filename);
        CodeWriter code_writer(filename);

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
            }
        }
    }

    return 0;
}