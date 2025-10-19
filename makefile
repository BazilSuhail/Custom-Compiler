# Makefile for Custom Compiler (auto-clean after build)

all: parser.exe clean

parser.tab.cpp parser.tab.hpp: parser.y
	bison -d -o parser.tab.cpp parser.y

lexer.o: lexer.cpp parser.tab.hpp
	g++ -c lexer.cpp -o lexer.o

parser.o: parser.tab.cpp
	g++ -c parser.tab.cpp -o parser.o

parser.exe: lexer.o parser.o
	g++ lexer.o parser.o -o parser.exe
	@echo "Build complete: parser.exe"

clean:
	rm -f *.o 
	@echo "Temporary files removed."

run:
	@echo "Running parser..."
	./parser.exe