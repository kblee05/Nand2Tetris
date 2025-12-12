#include <iostream>
#include <string>
#include <fstream>
#include <map>

std::map<std::string, std::string> memory_segment = {
    {"local", "LCL"},
    {"arguement", "ARG"},
    {"this", "THIS"},
    {"that", "THAT"}
};

void write_push_pop(std::ofstream& output_file, std::string operation, std::string segment_type, int index);

int main(){

    std::string filename = "StackTest"; 
    //std::cin >> filename; // "BasicTest.vm"

    std::ifstream input_file(filename + ".vm");
    std::ofstream output_file(filename + ".asm");

    std::string input;
    while(input_file >> input){
        if(input == "push" || input == "pop"){
            std::string segment_type;
            input_file >> segment_type;
            int index;
            input_file >> index;

            write_push_pop(output_file, input, segment_type, index);
        }
    }

    return 0;
}

void write_push_pop(std::ofstream& output_file, std::string operation, std::string segment_type, int index){

    // commment
    output_file << "// " << operation << " " << segment_type << " " << index << "\n";

    if(operation == "push"){

        if(segment_type == "constant"){
            output_file << "@" << index << "\n";
            output_file << "D=A\n";
        }
        else if(segment_type == "temp"){
            output_file << "@5\n";
            output_file << "D=A\n";
            output_file << "@" << index << "\n";
            output_file << "A=D+A\n";
            output_file << "D=M\n";
        }
        else{
            // Find Register
            output_file << "@" << memory_segment[segment_type] << "\n";
            output_file << "D=A\n";
            output_file << "@" << index << "\n";
            output_file << "A=D+A\n";
            output_file << "D=M\n";
        }

        // Push in Stack
        output_file << "@SP\n";
        output_file << "A=M\n";
        output_file << "M=D\n";
        output_file << "@SP\n";
        output_file << "M=M+1\n";
    }
    else if(operation == "pop"){
        // Store memory segment address
        output_file << "@" << memory_segment[segment_type] << "\n";
        output_file << "D=M\n";
        output_file << "@" << index << "\n";
        output_file << "@R13\n";
        output_file << "M=D\n";

        // Access Stack
        output_file << "@SP\n";
        output_file << "AM=M-1\n";
        output_file << "D=M\n";

        // Pop to sement address
        output_file << "@R13\n";
        output_file << "A=M\n";
        output_file << "M=D\n";
    }
}