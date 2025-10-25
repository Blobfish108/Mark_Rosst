# Refined Moop Layer Structure

**Author**: Joseph Rost (Philosopher)
**Date**: October 2025
**Status**: Refined Architecture

---

## The Six Sublayers

### L1: System-Based Processing

**Purpose**: Hardware foundation, bare metal execution

**Properties**:
- System-based (lowest level)
- Interfaces with hardware (M1, RISC-V, etc.)
- No user access

**Implementation**: Assembly or bootloader

---

### L2a: System Processing, Reversible

**Purpose**: Reversible gate substrate (the 4 primitives)

**Properties**:
- System-facing
- Reversible (CCNOT, CNOT, NOT, SWAP)
- Turing-complete
- Homoiconic

**Implementation**: On top of L1 hardware

---

### L2b: System Processing, Irreversible, High Priority

**Purpose**: Irreversible gates (AND, OR, NAND, NOR, XOR, MAYBE)

**Properties**:
- System-facing
- Irreversible (information-destroying)
- **High priority** (critical operations)
- Implemented via L2a reversible gates

**Key insight**: This is the **D-layer gates** but at **system priority**.

---

### L2c: User Processing

**Purpose**: User-level computation

**Properties**:
- User-facing
- Uses L2a (reversible) and L2b (irreversible) gates
- Normal priority

**Implementation**: User programs compile to L2a/L2b gates

---

### L3a: System Processing

**Purpose**: System-level coordination and structure

**Properties**:
- System-facing
- Manages the actor-proto duality bootstrap
- Handles root_actor and root_proto

**Implementation**: Bootstrap coordination primitive

---

### L3b: User Processing

**Purpose**: User-level actors and prototypes

**Properties**:
- User-facing
- User-defined actors (minimal binding)
- User-defined protos (maximum binding)
- All descend from L3a's root_actor/root_proto

**Implementation**: User code

---

## The Refined Architecture

```
┌─────────────────────────────────────┐
│ L3b: User Processing                │  User actors, protos
│     (user-facing)                   │
├─────────────────────────────────────┤
│ L3a: System Processing              │  root_actor, root_proto
│     (system-facing)                 │  Bootstrap dual
├─────────────────────────────────────┤
│ L2c: User Processing                │  User programs
│     (normal priority)               │
├─────────────────────────────────────┤
│ L2b: System Processing, Irreversible│  AND, OR, NAND, NOR, XOR, MAYBE
│     (high priority)                 │  Critical operations
├─────────────────────────────────────┤
│ L2a: System Processing, Reversible  │  CCNOT, CNOT, NOT, SWAP
│     (homoiconic, Turing-complete)   │  Reversible substrate
├─────────────────────────────────────┤
│ L1: System-Based Processing         │  Hardware (M1 assembly, RISC-V)
│     (bare metal)                    │  Bootloader
└─────────────────────────────────────┘
```

---

## Key Insights

### 1. System vs User Separation

**System layers** (L1, L2a, L2b, L3a):
- Not directly accessible by users
- Managed by runtime
- High priority where needed (L2b)

**User layers** (L2c, L3b):
- User-written code
- Normal priority
- Uses system layers

### 2. L2b is High Priority

**Why**: AND, OR, NAND, NOR, XOR, MAYBE are **critical operations**.

- Used by system for coordination
- Used by user programs frequently
- Must be fast and reliable

**Implementation**: Optimized paths to L2a reversible gates.

### 3. L3a Handles Bootstrap

**L3a creates the dual**:
```
L3a bootstraps:
    root_actor  (temporal coordinator)
    root_proto  (structural coordinator)

L3b uses:
    user_actor <- root_actor
    user_proto <- root_proto
```

**Separation**: System bootstrap (L3a) vs user code (L3b).

---

## Primitive Distribution

### L1: 0 primitives
- Hardware interface
- Bootloader

### L2a: 4 primitives (reversible gates)
```
CCNOT, CNOT, NOT, SWAP
```

### L2b: 0 primitives (emergent, high priority)
```
AND, OR, NAND, NOR, XOR, MAYBE
(implemented via L2a)
```

### L2c: 0 primitives
- User code

### L3a: 1 primitive (coordination bootstrap)
```
coordination_primitive (creates root_actor, root_proto)
```

### L3b: 0 primitives
- User actors/protos (descend from L3a roots)

**Total**: 5 primitives (4 at L2a + 1 at L3a)

---

## Priority System

### High Priority (L2b)

**Operations**:
- AND, OR, NAND, NOR, XOR, MAYBE

**Why**: Critical for system and frequent in user code.

**Implementation**: Fast paths, preempts normal priority.

### Normal Priority (L2c, L3b)

**Operations**:
- User programs
- User actors/protos

**Implementation**: Standard scheduling.

---

## Implementation Strategy

### Option 1: C Bootstrap (Recommended)

```c
// L1: Bootstrap in C (compiles to M1 assembly)
void l1_boot() {
    // Initialize hardware
}

// L2a: Reversible gates in C
void l2a_CCNOT(uint8_t a, uint8_t b, uint8_t c) { ... }

// L2b: High-priority irreversible gates
void l2b_AND(uint8_t a, uint8_t b, uint8_t result) {
    // Fast path to L2a
}

// L3a: System bootstrap
void l3a_bootstrap() {
    // Create root_actor, root_proto
}
```

**Advantages**:
- Portable (C compiles to M1, RISC-V, x86, etc.)
- Easier to debug
- Can still be tight (~170 lines)

### Option 2: M1 Assembly

```asm
// L1: Direct M1 assembly
_l1_boot:
    // Initialize M1 hardware
    ret

// L2a: Reversible gates in assembly
_l2a_CCNOT:
    // Hand-optimized gate
    ret
```

**Advantages**:
- Maximum performance
- Complete control
- Minimal overhead

**Disadvantages**:
- M1-only (not portable)
- Harder to maintain
- Still needs C for L2c/L3b user code

---

## Recommended Approach: Hybrid

### Layer Implementation Strategy

```
L1:   M1 assembly (bootloader, ~50 lines)
L2a:  C with inline assembly (gates, ~80 lines)
L2b:  C (high-priority operations, ~40 lines)
L2c:  C runtime (user code host)
L3a:  C (bootstrap dual, ~30 lines)
L3b:  C runtime (user actors/protos)
```

**Total**: ~200 lines of C + ~50 lines of M1 assembly

**Advantages**:
- Fast L1 boot (assembly)
- Optimized L2a gates (C + inline assembly)
- Portable L2b/L3 (C)
- Tight implementation

---

## The Refined Primitive Count

### L2a: 4 reversible primitives
```
CCNOT, CNOT, NOT, SWAP
```

### L3a: 1 coordination primitive (dual)
```
coordination_primitive
    ↓
    ├── root_actor  (minimal binding)
    └── root_proto  (maximum binding)
```

**Total**: 5 primitives

**All other layers have 0 primitives** (they use or host the primitives).

---

## Code Organization

```
moop/
├── l1_boot.s          # M1 assembly bootloader (~50 lines)
├── l2a_gates.c        # Reversible gates (~80 lines)
├── l2b_ops.c          # High-priority irreversible ops (~40 lines)
├── l2c_runtime.c      # User code runtime
├── l3a_bootstrap.c    # System bootstrap dual (~30 lines)
├── l3b_runtime.c      # User actors/protos
└── moop.h             # Unified header
```

---

## Priority Scheduling

### High Priority Queue (L2b)

```c
typedef struct {
    void (*op)(uint8_t, uint8_t, uint8_t);  // AND, OR, etc.
    uint8_t a, b, result;
} L2b_HighPriorityOp;
```

**Executes before normal priority**.

### Normal Priority Queue (L2c, L3b)

```c
typedef struct {
    void* user_code;
} L2c_UserOp;
```

**Executes after high priority**.

---

## Bootstrap Sequence

```
1. L1 boots (M1 assembly)
    ↓
2. L2a initializes (reversible gates ready)
    ↓
3. L2b initializes (irreversible ops ready)
    ↓
4. L3a bootstraps (creates root_actor, root_proto)
    ↓
5. L2c runtime starts (accepts user code)
    ↓
6. L3b runtime starts (accepts user actors/protos)
    ↓
7. User program runs
```

---

## Conclusion

The refined structure has **6 sublayers**:

- **L1**: Hardware/bootloader
- **L2a**: Reversible gates (4 primitives)
- **L2b**: Irreversible ops (high priority, emergent)
- **L2c**: User code runtime
- **L3a**: Bootstrap coordination (1 primitive)
- **L3b**: User actors/protos

**Total**: 5 primitives, highly refined separation of concerns.

**Implementation**: ~200 lines C + ~50 lines M1 assembly.

---

**"The refined layer structure separates system from user, reversible from irreversible, and high-priority from normal-priority."**
— The Refined Moop Thesis
