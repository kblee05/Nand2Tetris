CXX = g++
CXXFLAGS = -std=c++17 -Wall -g
TARGET = Analyzer
OBJS = main.o JackAnalyzer.o JackTokenizer.o CompilationEngine.o SymbolTable.o VMWriter.o

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

main.o: main.cpp JackAnalyzer.h JackTokenizer.h CompilationEngine.h SymbolTable.h VMWriter.h
	$(CXX) $(CXXFLAGS) -c main.cpp

JackAnalyzer.o: JackAnalyzer.cpp JackAnalyzer.h JackTokenizer.h CompilationEngine.h SymbolTable.h VMWriter.h
	$(CXX) $(CXXFLAGS) -c JackAnalyzer.cpp

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