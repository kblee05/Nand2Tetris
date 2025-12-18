VMT: main.o Parser.o CodeWriter.o
	g++ -o VMT main.o Parser.o CodeWriter.o

main.o: main.cpp
	g++ -c main.cpp

Parser.o: Parser.cpp
	g++ -c Parser.cpp

CodeWriter.o: CodeWriter.cpp
	g++ -c CodeWriter.cpp

clean:
	rm -f *.o VMT	