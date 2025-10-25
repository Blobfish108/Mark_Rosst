#!/bin/bash
# Verification script to test build with both compilers
# This simulates what the CI does

set -e  # Exit on error

echo "╔════════════════════════════════════════════════════════════╗"
echo "║  Unified Moop: Local Build Verification                   ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

# Test with GCC
echo "=== Testing with GCC ==="
export CC=gcc
echo "CC is set to: $CC"
$CC --version | head -1
make clean
make
make test
echo "✓ GCC build and tests passed"
echo ""

# Test with Clang
echo "=== Testing with Clang ==="
export CC=clang
echo "CC is set to: $CC"
$CC --version | head -1
make clean
make
make test
echo "✓ Clang build and tests passed"
echo ""

echo "╔════════════════════════════════════════════════════════════╗"
echo "║  ✓ All builds verified successfully!                      ║"
echo "╚════════════════════════════════════════════════════════════╝"
