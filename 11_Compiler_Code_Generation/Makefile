CXX = g++
CXXFLAGS = -std=c++17 -g
TARGET = Compiler
OBJS = main.o JackCompiler.o JackTokenizer.o CompilationEngine.o SymbolTable.o VMWriter.o

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

main.o: main.cpp JackCompiler.h JackTokenizer.h CompilationEngine.h SymbolTable.h VMWriter.h
	$(CXX) $(CXXFLAGS) -c main.cpp

JackCompiler.o: JackCompiler.cpp JackCompiler.h JackTokenizer.h CompilationEngine.h SymbolTable.h VMWriter.h
	$(CXX) $(CXXFLAGS) -c JackCompiler.cpp

CompilationEngine.o: CompilationEngine.cpp CompilationEngine.h JackTokenizer.h SymbolTable.h VMWriter.h
	$(CXX) $(CXXFLAGS) -c CompilationEngine.cpp

JackTokenizer.o: JackTokenizer.cpp JackTokenizer.h
	$(CXX) $(CXXFLAGS) -c JackTokenizer.cpp

SymbolTable.o: SymbolTable.cpp SymbolTable.h Types.h
	$(CXX) $(CXXFLAGS) -c SymbolTable.cpp

VMWriter.o: VMWriter.cpp VMWriter.h
	$(CXX) $(CXXFLAGS) -c VMWriter.cpp

clean:
	rm -f *.o $(TARGET)