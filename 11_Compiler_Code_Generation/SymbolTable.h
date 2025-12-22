#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_h

#include <string>
#include "Types.h"
#include <unordered_map>

class SymbolTable{
public:
    enum class Kind{
        STATIC,
        FIELD,
        ARG,
        VAR,
        NONE
    };
private:
    struct SymbolInfo{
        std::string type;
        Kind kind;
        int index;
    };

    std::unordered_map<std::string, SymbolInfo> class_table;
    std::unordered_map<std::string, SymbolInfo> subroutine_table;
    std::unordered_map<Kind, int> indices;
    SymbolInfo find_symbol_info(std::string name);
public:
    SymbolTable();
    void reset();
    void define(std::string name, std::string type, Kind kind);
    int get_var_count(Kind kind);
    Kind get_kind(std::string name);
    std::string get_type(std::string name);
    int get_index(std::string name);
    bool exists(std::string name);
};

#endif