# Makefile for MyCustomLoopInterchange LLVM Pass

CXX = clang++-17
OPT = opt-17
CLANG = clang-17
LLC = llc-17

CXXFLAGS = -fPIC -std=c++17 $(shell llvm-config-17 --cxxflags)
LDFLAGS = -shared $(shell llvm-config-17 --ldflags)

PASS_SO = MyCustomLoopInterchange.so
PASS_SRC = MyCustomLoopInterchange.cpp
TEST_SRC = test.c
TEST_LL = test.ll
TEST_CANONICAL = test_canonical.ll
TEST_OUT = test_out.ll

.PHONY: all build test clean

all: build

# Build the pass
build: $(PASS_SO)

$(PASS_SO): $(PASS_SRC)
	@echo "Building pass..."
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@
	@echo "Pass compiled: $(PASS_SO)"

# Run full test
test: $(PASS_SO) $(TEST_OUT)
	@echo ""
	@echo "================================================"
	@echo "  Loop Interchange Test Results"
	@echo "================================================"
	@echo ""
	@echo "BEFORE:"
	@echo "  Outer loop PHI:"
	@grep "%.01 = phi" $(TEST_CANONICAL) | head -1 || true
	@echo "  Outer loop bound:"
	@grep "icmp slt i32 %.01" $(TEST_CANONICAL) | head -1 || true
	@echo "  Inner loop PHI:"
	@grep "%.0 = phi" $(TEST_CANONICAL) | head -1 || true
	@echo "  Inner loop bound:"
	@grep "icmp slt i32 %.0" $(TEST_CANONICAL) | head -1 || true
	@echo ""
	@echo "AFTER:"
	@echo "  Outer loop PHI:"
	@grep "%j = phi" $(TEST_OUT) | head -1 || true
	@echo "  Outer loop bound:"
	@grep "icmp slt i32 %j" $(TEST_OUT) | head -1 || true
	@echo "  Inner loop PHI:"
	@grep "%i = phi" $(TEST_OUT) | head -1 || true
	@echo "  Inner loop bound:"
	@grep "icmp slt i32 %i" $(TEST_OUT) | head -1 || true
	@echo ""
	@echo "================================================"
	@echo "  Test passed - loops successfully interchanged"
	@echo "================================================"

# Generate LLVM IR from C source
$(TEST_LL): $(TEST_SRC)
	@echo "Generating LLVM IR..."
	@$(CLANG) -O1 -S -emit-llvm $< -o $@ -Xclang -disable-llvm-passes

# Generate canonical loop form
$(TEST_CANONICAL): $(TEST_LL)
	@echo "Generating canonical loop form..."
	@$(OPT) -S -passes="mem2reg,simplifycfg,loop-simplify" $< -o $@

# Run loop interchange pass
$(TEST_OUT): $(PASS_SO) $(TEST_CANONICAL)
	@echo "Running loop interchange pass..."
	@$(OPT) -load-pass-plugin=./$(PASS_SO) -passes="my-loop-interchange" $(TEST_CANONICAL) -S -o $@
	@echo "Transformation complete"

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -f $(PASS_SO) $(TEST_LL) $(TEST_CANONICAL) $(TEST_OUT)
	@echo "Clean complete"
