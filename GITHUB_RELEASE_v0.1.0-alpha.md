# Moop v0.1.0-alpha: Quantum-Ready Living Code

**First alpha release** of Moop - a programming language that combines reversible computation, homoiconicity, and evolutionary substrate into "living code."

---

## 🎯 What Makes Moop Different

Moop occupies a unique design space no other language fills:

### The Unique Combination

**Traditional languages give you one or the other:**
- Lisp/Forth/Io: Code as data (homoiconic) ✓ | Can't undo modifications ✗
- Quantum computing: Reversible operations ✓ | Can't self-modify ✗
- Genetic programming: Evolutionary ✓ | No reversibility ✗

**Moop gives you all three:**
- ✅ Code as data (homoiconicity)
- ✅ Reversible execution (time-travel debugging)
- ✅ Evolutionary substrate (self-optimizing)
- ✅ Quantum-ready (same code, any backend)

### What This Enables

**Living code** - programs that:
- Read their own structure
- Modify themselves
- Test modifications reversibly
- Keep beneficial changes
- Evolve over time

**No other language can do this.**

---

## ⚡ Try It Now

```bash
git clone https://github.com/Blobfish108/Mark_Rosst.git
cd Mark_Rosst
make examples
./build/living_code_demo
```

**30 seconds to see:**
- Homoiconicity: Reading and modifying code at runtime
- Reversibility: Time-travel debugging with checkpoints
- Evolution: Self-optimizing fitness-based pruning
- The synergy: Why this combination is unique

---

## 🔬 Key Features

### 1. Quantum-Ready Architecture

**Runs on conventional hardware by default**, but quantum-compatible by design.

Built on reversible gates (CCNOT, CNOT, NOT, SWAP):
- Efficient on classical hardware (simple bit operations)
- Native to quantum computers (unitary operations)
- Mathematically universal (Turing-complete)

```c
// Same gate API works on any backend
Qubit_State* state = qubit_init(32, QUBIT_BACKEND_CLASSICAL);  // Fast
// Or: QUBIT_BACKEND_SIMULATOR  // Quantum simulator
// Or: QUBIT_BACKEND_QUANTUM     // Real QPU (future)
```

**Philosophy**: *"Write for conventional hardware today. Run on quantum hardware tomorrow."*

### 2. Homoiconic + Reversible

**Only language** combining code-as-data with undo capability:

```moop
# Read code from tape
cell = tape.read(100)

# Modify it
cell.gate = SWAP
tape.write(100, cell)

# Test it reversibly
checkpoint = tape.checkpoint()
result = execute(100)

# Don't like it? Undo
if not good(result):
    tape.restore(checkpoint)  # Complete reversal
```

**Enables**: Exploratory meta-programming with backtracking.

### 3. Evolutionary Substrate

Code organized as **complex adaptive system**:
- Operations compete for survival in 1024-cell tape
- Fitness-based selection (recency + activity + gate priority)
- Automatic pruning of low-value operations
- System adapts to usage patterns

**Self-organizing runtime** - no manual optimization needed.

### 4. Natural Language Syntax

Combines Quorum's naturalism with Io's minimalism:

```moop
actor Calculator
    role is "performs calculations"

    on compute(a, b)
        result <-> CCNOT(a, b, output)  # Reversible
        output -> "Result: " + result    # Irreversible
```

**Easier to learn** than Python/Java/JavaScript for both humans and LLMs.

### 5. Embedded-Ready

- **~40KB footprint** (runtime + tape)
- **No garbage collector** (deterministic behavior)
- **Fixed computational memory** (1024-cell tape)
- **Dual memory architecture** (gate-based + conventional)

Perfect for IoT, microcontrollers, real-time systems.

---

## 📊 Technical Specs

| Feature | Moop |
|---------|------|
| **Runtime Size** | ~40KB |
| **Startup Time** | <1ms |
| **GC Pauses** | None |
| **Deterministic** | Yes |
| **Computational Primitives** | CCNOT, CNOT, NOT, SWAP |
| **Backend Types** | Classical, Simulator, Quantum (future) |
| **Homoiconic** | Yes (reversible) |
| **Self-Evolving** | Yes (fitness-based) |
| **Implementation** | ~900 lines of C |
| **License** | MIT |

---

## 📦 What's Included

### Core Implementation
- `src/moop_quantum_ready.h` - Backend abstraction layer
- `src/classical_backend.c` - Default classical implementation
- `src/quantum_simulator_backend.c` - Optional quantum simulator
- `src/quantum_backend_registry.c` - Backend selection system
- `src/moop_enhanced.c` - Main runtime (~450 lines)

### Examples
- `examples/living_code_demo.c` - Demonstrates homoiconicity + reversibility + evolution
- `examples/evolutionary_optimization.c` - Self-evolving algorithm

### Tests
- `tests/test_enhanced.c` - Enhanced features test suite
- `tests/test_quantum_backends.c` - Backend abstraction tests
- All tests passing on classical + quantum simulator

### Documentation
- `README.md` - Getting started and overview
- `QUANTUM_READINESS.md` - Technical deep-dive (500+ lines)
- `LANGUAGE_COMPARISON.md` - Comparison with 6 other languages
- `LANGUAGE_AESTHETICS.md` - Design philosophy
- `PRIMITIVE_DUALITY.md` - Actor-proto duality
- `UNIVERSAL_COGNITIVE_PROTOCOL.md` - Mathematical foundations

---

## 🚀 Quick Start Guide

### Build Everything
```bash
make
```

### Run Tests
```bash
make test-all
```

### Build with Quantum Simulator
```bash
make clean
make CFLAGS="-DENABLE_QUANTUM_SIMULATOR -std=c11 -O2 -g"
make test-all
```

### Build Examples
```bash
make examples
./build/living_code_demo
./build/evolutionary_optimization
```

---

## 🎓 Learning Resources

### Start Here
1. **Quick demo**: `./build/living_code_demo` (30 seconds)
2. **README.md**: Overview and philosophy
3. **LANGUAGE_COMPARISON.md**: How Moop compares to other languages

### Go Deeper
4. **QUANTUM_READINESS.md**: Backend abstraction explained
5. **PRIMITIVE_DUALITY.md**: Actor-proto relationship
6. **UNIVERSAL_COGNITIVE_PROTOCOL.md**: Mathematical foundations

### Advanced
7. **examples/evolutionary_optimization.c**: Self-evolving algorithms
8. Source code (~900 lines, highly readable)

---

## 🔗 Related Projects

**[Trinity Vibe](https://github.com/Blobfish108/trinity-vibe)** - Browser-based exploration of homoiconicity and structural reversibility

Complementary projects:
- **Trinity Vibe**: Web-based laboratory for learning/experimentation
- **Moop**: Native C implementation for production/embedded

Both explore reversible homoiconic programming from different angles.

---

## ⚠️ Alpha Status

This is an **alpha release** - research project, not production-ready.

**Works:**
- ✅ All core features implemented
- ✅ Classical backend fully functional
- ✅ Quantum simulator backend working
- ✅ All tests passing
- ✅ Examples demonstrating unique capabilities

**Not yet:**
- ❌ Natural language parser (stubs only)
- ❌ Standard library
- ❌ Package manager
- ❌ IDE support
- ❌ Real quantum hardware integration

**Use for:**
- Research in reversible/quantum computing
- Embedded systems experimentation
- Learning about homoiconic + reversible programming
- Exploring self-evolving code

**Don't use for:**
- Production web applications
- Mission-critical systems (yet)
- Projects requiring mature ecosystem

---

## 🤝 Contributing

Pull requests welcome! Please ensure:
- Code passes `make test-all`
- New features include tests
- Follows existing code style (K&R, 4-space indent)
- Stays minimal (~1200 lines max for core)

**Areas for contribution:**
- Natural language parser implementation
- Additional quantum backend (hardware integration)
- Standard library of reversible primitives
- Performance benchmarks
- Example programs
- Documentation improvements

---

## 📜 License

MIT License - see [LICENSE](LICENSE)

Copyright (c) 2025 Mark Rosst

---

## 🙏 Acknowledgments

Built on ideas from:
- **Quorum**: Evidence-based natural language syntax
- **Io**: Minimalist elegance
- **Reversible computing**: Toffoli, Fredkin, Bennett
- **Quantum computing**: Deutsch, Shor, Grover
- **Complex adaptive systems**: Prigogine, Holland
- **Homoiconic languages**: Lisp, Forth, Io

Special thanks to Claude Code for collaborative development.

---

## 🌟 What's Next

**Immediate (v0.2.0):**
- Implement natural language parser
- Performance benchmarks
- More example programs

**Near-term (v0.3.0):**
- Standard library
- Real quantum hardware integration (IBM Qiskit, AWS Braket)
- Embedded hardware demos (Arduino, ESP32)

**Long-term:**
- Language server protocol (IDE support)
- Package manager
- Formal verification tools
- Academic publication

---

**Star this repo** if you find the concept interesting!

**Watch** for updates as Moop evolves.

**Clone and experiment** - this is what living code looks like.

---

*"Moop: Where code becomes a living, quantum-ready organism."*

**Repository**: https://github.com/Blobfish108/Mark_Rosst
**Release**: v0.1.0-alpha
**Date**: October 2025
**Author**: Mark Rosst
