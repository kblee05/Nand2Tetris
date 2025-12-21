#ifndef VMWRITER_H
#define VMWRITER_H

#include <string>
#include <fstream>

class VMWriter{
public:
    enum class Segment{
        CONST,
        ARG,
        LOCAL,
        STATIC,
        THIS,
        THAT,
        POINTER,
        TEMP
    };

    enum class Command{
        ADD, SUB, NEG,
        EQ, GT, LT,
        AND, OR, NOT
    };
private:
    std::ofstream& output;
    std::string get_segment_str(Segment segment);
public:
    VMWriter(std::ofstream& ofstream);
    void write_push(Segment segment, int index);
    void write_pop(Segment segment, int index);
    void write_arithmetic(Command command);
    void write_label(std::string label);
    void write_goto(std::string label);
    void write_if(std::string label);
    void write_call(std::string name, int num_of_args);
    void write_function(std::string name, int num_of_vars);
    void write_return();
    void close();
};

#endif