# Makefile for Unified Moop
# Minimal, elegant build system for evolutionary tape-loop Turing machine

CC ?= gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -O2 -g
LIBS = -lm

# Optional: LLM integration (requires libcurl)
# Uncomment to enable LLM proto
# CFLAGS += -DENABLE_LLM
# LIBS += -lcurl

# Optional: Quantum simulator backend
# Uncomment to enable quantum statevector simulation
# CFLAGS += -DENABLE_QUANTUM_SIMULATOR

SRCDIR = src
TESTDIR = tests
BUILDDIR = build

# Core sources (enhanced implementation with quantum-ready abstraction)
CORE_SRCS = $(SRCDIR)/moop_enhanced.c \
            $(SRCDIR)/classical_backend.c \
            $(SRCDIR)/quantum_backend_registry.c

CORE_OBJS = $(BUILDDIR)/moop_enhanced.o \
            $(BUILDDIR)/classical_backend.o \
            $(BUILDDIR)/quantum_backend_registry.o

# Optional quantum simulator backend
ifeq ($(findstring -DENABLE_QUANTUM_SIMULATOR,$(CFLAGS)),-DENABLE_QUANTUM_SIMULATOR)
CORE_SRCS += $(SRCDIR)/quantum_simulator_backend.c
CORE_OBJS += $(BUILDDIR)/quantum_simulator_backend.o
endif

# Test sources
TEST_SRCS = $(TESTDIR)/test_enhanced.c
TEST_TARGET = $(BUILDDIR)/test_enhanced
TEST_QUANTUM_SRCS = $(TESTDIR)/test_quantum_backends.c
TEST_QUANTUM_TARGET = $(BUILDDIR)/test_quantum_backends

# Example programs
EXAMPLES_DIR = examples
EXAMPLE_EVOLUTIONARY = $(BUILDDIR)/evolutionary_optimization
EXAMPLE_LIVING_CODE = $(BUILDDIR)/living_code_demo

.PHONY: all clean test test-quantum test-all examples help

all: $(BUILDDIR) $(TEST_TARGET) $(TEST_QUANTUM_TARGET)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/moop_enhanced.o: $(SRCDIR)/moop_enhanced.c $(SRCDIR)/moop_enhanced.h $(SRCDIR)/moop_quantum_ready.h | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/classical_backend.o: $(SRCDIR)/classical_backend.c $(SRCDIR)/moop_quantum_ready.h | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/quantum_backend_registry.o: $(SRCDIR)/quantum_backend_registry.c $(SRCDIR)/moop_quantum_ready.h | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/quantum_simulator_backend.o: $(SRCDIR)/quantum_simulator_backend.c $(SRCDIR)/moop_quantum_ready.h | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_TARGET): $(TEST_SRCS) $(CORE_OBJS) | $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(TEST_QUANTUM_TARGET): $(TEST_QUANTUM_SRCS) $(CORE_OBJS) | $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(EXAMPLE_EVOLUTIONARY): $(EXAMPLES_DIR)/evolutionary_optimization.c $(CORE_OBJS) | $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(EXAMPLE_LIVING_CODE): $(EXAMPLES_DIR)/living_code_demo.c $(CORE_OBJS) | $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

examples: $(EXAMPLE_EVOLUTIONARY) $(EXAMPLE_LIVING_CODE)
	@echo "Examples built successfully"

test: $(TEST_TARGET)
	@echo "=== Running Enhanced Moop Test Suite ==="
	./$(TEST_TARGET)

test-quantum: $(TEST_QUANTUM_TARGET)
	@echo "=== Running Quantum-Ready Backend Test Suite ==="
	./$(TEST_QUANTUM_TARGET)

test-all: $(TEST_TARGET) $(TEST_QUANTUM_TARGET)
	@echo "=== Running All Test Suites ==="
	./$(TEST_TARGET)
	./$(TEST_QUANTUM_TARGET)

clean:
	rm -rf $(BUILDDIR)
	@echo "Build artifacts cleaned"

help:
	@echo "Unified Moop Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Build all test suites (default)"
	@echo "  test         - Run enhanced features test suite"
	@echo "  test-quantum - Run quantum backend test suite"
	@echo "  test-all     - Run all test suites"
	@echo "  examples     - Build example programs"
	@echo "  clean        - Remove build artifacts"
	@echo "  help         - Show this message"
	@echo ""
	@echo "Features:"
	@echo "  - Quantum-ready abstraction (classical default)"
	@echo "  - Tape-loop Turing machine (1024 cells)"
	@echo "  - Evolutionary pruning with fitness"
	@echo "  - Trinary MAYBE with LLM confidence"
	@echo "  - Self-modification API (homoiconicity)"
	@echo "  - Meta-evolution (adaptive fitness tuning)"
	@echo ""
	@echo "Build Options:"
	@echo "  Classical only (default):"
	@echo "    make"
	@echo ""
	@echo "  Enable quantum simulator:"
	@echo "    make clean"
	@echo "    make CFLAGS=\"-DENABLE_QUANTUM_SIMULATOR -std=c11 -O2 -g\""
	@echo ""
	@echo "  Run quantum tests:"
	@echo "    make test-all"
	@echo ""
	@echo "Examples:"
	@echo "  Build examples:"
	@echo "    make examples"
	@echo ""
	@echo "  Run living code demo (recommended first):"
	@echo "    ./build/living_code_demo"
	@echo ""
	@echo "  Run evolutionary optimization:"
	@echo "    ./build/evolutionary_optimization"
