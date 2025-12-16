VMT: main.o Parser.o CodeWriter.o
	g++ -o VMT main.o Parser.o CodeWriter.o

main.o: main.cpp Parser.h CodeWriter.h
	g++ -c main.cpp

Parser.o: Parser.cpp Parser.h
	g++ -c Parser.cpp

CodeWriter.o: CodeWriter.cpp CodeWriter.h
	g++ -c CodeWriter.cpp

clean:
	rm -f *.o VMT