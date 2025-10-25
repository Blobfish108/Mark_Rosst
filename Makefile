# Makefile for Unified Moop
# Minimal, elegant build system for evolutionary tape-loop Turing machine

CC ?= gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -O2 -g
LIBS = -lm

# Optional: LLM integration (requires libcurl)
# Uncomment to enable LLM proto
# CFLAGS += -DENABLE_LLM
# LIBS += -lcurl

SRCDIR = src
TESTDIR = tests
BUILDDIR = build

# Core sources (enhanced implementation)
CORE_SRCS = $(SRCDIR)/moop_enhanced.c
CORE_OBJS = $(BUILDDIR)/moop_enhanced.o

# Test sources
TEST_SRCS = $(TESTDIR)/test_enhanced.c
TEST_TARGET = $(BUILDDIR)/test_enhanced

.PHONY: all clean test examples help

all: $(BUILDDIR) $(TEST_TARGET)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/moop_enhanced.o: $(SRCDIR)/moop_enhanced.c $(SRCDIR)/moop_enhanced.h | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_TARGET): $(TEST_SRCS) $(CORE_OBJS) | $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

test: $(TEST_TARGET)
	@echo "=== Running Enhanced Moop Test Suite ==="
	./$(TEST_TARGET)

clean:
	rm -rf $(BUILDDIR)
	@echo "Build artifacts cleaned"

help:
	@echo "Unified Moop Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build test suite (default)"
	@echo "  test     - Build and run tests"
	@echo "  clean    - Remove build artifacts"
	@echo "  help     - Show this message"
	@echo ""
	@echo "Features:"
	@echo "  - Tape-loop Turing machine (1024 cells)"
	@echo "  - Evolutionary pruning with fitness"
	@echo "  - Trinary MAYBE with LLM confidence"
	@echo "  - Self-modification API (homoiconicity)"
	@echo "  - Meta-evolution (adaptive fitness tuning)"
