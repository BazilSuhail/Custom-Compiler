# === Compiler and flags ===
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# LLVM flags (for llvm_test.cpp)
LLVM_FLAGS = $(shell llvm-config --cxxflags --ldflags --libs all --system-libs)

# Source files for main compiler
COMPILER_SOURCES = lexer.cpp parser.cpp scope.cpp type.cpp tac.cpp main.cpp

# Output executables
COMPILER_OUTPUT = compiler.exe
LLVM_OUTPUT = llvm_program.exe

# Default input file
INPUT ?= sample

# === Targets ===

# Build and run main compiler
all: $(COMPILER_OUTPUT)
	@echo Running $(COMPILER_OUTPUT) with input file tester/$(INPUT).txt...
	./$(COMPILER_OUTPUT) $(INPUT)

$(COMPILER_OUTPUT): $(COMPILER_SOURCES)
	@echo Compiling compiler sources...
	$(CXX) $(CXXFLAGS) $(COMPILER_SOURCES) -o $(COMPILER_OUTPUT)
	@echo Compilation finished.

# Run main compiler only
run: $(COMPILER_OUTPUT) $(LLVM_OUTPUT)
	@echo Running $(COMPILER_OUTPUT) with input file tester/$(INPUT).txt...
	./$(COMPILER_OUTPUT) $(INPUT)
	@echo Running $(LLVM_OUTPUT)...
	./$(LLVM_OUTPUT)

# Compile and run LLVM test program
llvm: llvm_test.cpp
	@echo Compiling llvm_test.cpp with LLVM...
	$(CXX) $(CXXFLAGS) llvm_test.cpp -o $(LLVM_OUTPUT) $(LLVM_FLAGS)
	@echo Running $(LLVM_OUTPUT)...
	./$(LLVM_OUTPUT)

# Clean all executables
clean:
	rm -f $(COMPILER_OUTPUT) $(LLVM_OUTPUT)
