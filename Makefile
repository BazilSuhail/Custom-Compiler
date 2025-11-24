# === Compiler and flags ===
CXX = g++
CXXFLAGS = -std=c++17 -Wall -fexceptions

# LLVM flags (used automatically if llvm_test.cpp is included)
LLVM_FLAGS = $(shell llvm-config --cxxflags --ldflags --libs all --system-libs)

# Source files (main.cpp calls or includes everything)
SOURCES = lexer.cpp parser.cpp scope.cpp type.cpp tac.cpp llvm.cpp main.cpp

# Output executable
OUTPUT = program.exe

# Default input file
INPUT ?= sample

# === Targets ===

# Default: do nothing
all:
	@echo "Use 'make build' to compile the program OR 'make run' to run it"

# Build the executable
build: $(OUTPUT)

$(OUTPUT): $(SOURCES)
	@echo Compiling all sources into $(OUTPUT)...
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(OUTPUT) $(LLVM_FLAGS) -fexceptions
	@echo Compilation finished.

# Run the program with optional input file
run: $(OUTPUT)
	@echo Running $(OUTPUT) with input file tester/$(INPUT).txt...
	./$(OUTPUT) $(INPUT)

# Clean executable
clean:
	rm -f $(OUTPUT)
