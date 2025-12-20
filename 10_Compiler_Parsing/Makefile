Analyzer: main.o JackAnalyzer.o JackTokenizer.o CompilationEngine.o
	g++ -std=c++17 -o Analyzer JackAnalyzer.o main.o JackTokenizer.o CompilationEngine.o

main.o: main.cpp JackAnalyzer.h JackTokenizer.h CompilationEngine.h
	g++ -std=c++17 -c main.cpp

JackAnalyzer.o: JackAnalyzer.cpp JackAnalyzer.h JackTokenizer.h CompilationEngine.h
	g++ -c JackAnalyzer.cpp

CompilationEngine.o: CompilationEngine.cpp CompilationEngine.h JackTokenizer.h
	g++ -c CompilationEngine.cpp

JackTokenizer.o: JackTokenizer.cpp JackTokenizer.h
	g++ -c JackTokenizer.cpp