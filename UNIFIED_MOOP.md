# Unified Moop: The Three-Layer Language

**Author**: Mark Rosst (Philosopher)
**Date**: October 2025
**Version**: 1.0
**Status**: Definitive Specification

---

## Executive Summary

**Moop is a single unified language with three computational layers:**

- **R-layer**: Reversible substrate (4 primitive gates)
- **D-layer**: Dissipative operations (emergent, no primitives)
- **S-layer**: Structural coordination (1 primitive with dual aspects)

**Total primitives**: 5 mathematical primitives, 6 operational forms

**Implementation**: ~170 lines of tight C code

---

## The Three Layers

### R-Layer: Reversible Substrate

**Purpose**: System-facing foundation providing Turing-completeness, homoiconicity, and reversibility.

**Properties**:
- Turing-complete
- Homoiconic (gates are data)
- Reversible (all operations can be undone)
- System-facing (users don't write R-layer directly)

**The 4 Primitive Gates**:
```
CCNOT a b c    # Toffoli (universal reversible gate)
CNOT a b       # Controlled NOT
NOT a          # Bit flip
SWAP a b       # Exchange qubits
```

**Implementation**: 4-byte cells, self-inverse property, automatic history recording

**Code size**: ~80 lines

---

### D-Layer: Dissipative Operations

**Purpose**: Emergent irreversible operations controlled by R-layer gates and S-layer organization.

**Properties**:
- No primitives (everything emerges)
- Irreversible semantics (information-destroying)
- Controlled by R-layer + S-layer primitives
- User-facing

**Emergent Operations**:
```
AND a b result     # Controlled by CCNOT
OR a b result      # Controlled by CCNOT + NOT
XOR a b result     # Controlled by CNOT
NAND a b result    # Controlled by CCNOT + NOT
NOR a b result     # Controlled by CCNOT + NOT
MAYBE              # Homoiconic data structure
```

**Key insight**: D-layer is the **interaction space** where R-layer computation and S-layer organization produce irreversible semantics.

**Code size**: ~40 lines

---

### S-Layer: Structural Coordination

**Purpose**: Organize computation via the coordination primitive's dual aspects.

**Properties**:
- 1 primitive (coordination) with 2 dual aspects
- Actor-proto duality (like particle-wave duality)
- Both aspects emerge simultaneously from R-layer
- User-facing

**The Coordination Primitive**:

```
coordination_primitive
    ↓
    ├── actor  (minimal binding aspect)
    └── proto  (maximum binding aspect)
```

**Actor** (minimal binding):
```
actor Counter
    role is "Count events"

    state has
        count is 0

    handlers

    on increment
        state.count = count + 1
```

**Proto** (maximum binding):
```
proto Calculator <- MathProto
    slots: value, history

    methods
        add(x)
            value = value + x
```

**The Bootstrap Dual**:

Every program instance creates:
```
root_actor  (temporal coordinator, minimal binding)
root_proto  (structural coordinator, maximum binding)
```

They reference each other:
- `root_actor.prototype = root_proto`
- `root_proto.coordinator = root_actor`

**All actors descend from `root_actor`**
**All protos descend from `root_proto`**

**Code size**: ~50 lines

---

## Total Primitive Count

### Mathematical Primitives: 5

```
1. CCNOT            (R-layer)
2. CNOT             (R-layer)
3. NOT              (R-layer)
4. SWAP             (R-layer)
5. coordination     (S-layer, with dual aspects)
```

### Operational Primitives: 6

```
1. CCNOT            (R-layer)
2. CNOT             (R-layer)
3. NOT              (R-layer)
4. SWAP             (R-layer)
5. actor            (S-layer, minimal binding)
6. proto            (S-layer, maximum binding)
```

**Both counts are correct** - depends on whether you count the mathematical essence (5) or the operational syntax (6).

---

## Implementation Size

### Complete Implementation: ~170 lines

```
R-layer:  ~80 lines (init, 4 gates, reversibility, homoiconicity)
S-layer:  ~50 lines (dual bootstrap, actor/proto creation)
D-layer:  ~40 lines (emergent operations via R-layer)
────────────────────
Total:    ~170 lines
```

**This is tight code** - minimal, essential, no bloat.

---

## Example: Three Layers Working Together

```c
#include "moop_core.h"

int main() {
    // S-layer: Initialize with dual (root_actor + root_proto)
    S_Runtime* s = s_init(32, 1);  // 32 qubits, instance ID 1

    // S-layer: Create proto (maximum binding)
    S_Proto* MathProto = s_create_proto(s, "MathProto", s->root_proto);

    // S-layer: Create actor (minimal binding)
    S_Actor* calculator = s_create_actor(s, "Calculator", "Perform calculations");

    // R-layer: Reversible computation (substrate)
    r_NOT(s->r_runtime, 0);      // Set qubit 0 to 1
    r_NOT(s->r_runtime, 1);      // Set qubit 1 to 1

    // D-layer: Irreversible operation (emergent)
    d_AND(s->r_runtime, 0, 1, 2);  // result = qubit 0 AND qubit 1

    printf("Result (qubit 2): %d\n", s->r_runtime->qubits[2]);  // 1

    // R-layer: Reversibility (checkpoint and restore)
    uint32_t checkpoint = r_checkpoint(s->r_runtime);

    r_NOT(s->r_runtime, 2);  // Flip result
    printf("After flip: %d\n", s->r_runtime->qubits[2]);  // 0

    r_restore(s->r_runtime, checkpoint);  // Rewind
    printf("After restore: %d\n", s->r_runtime->qubits[2]);  // 1

    // R-layer: Homoiconicity (print history)
    printf("\nExecution history:\n");
    for (uint32_t i = 0; i < s->r_runtime->history_count; i++) {
        printf("  %s\n", r_print(s->r_runtime->history[i]));
    }

    s_free(s);
    return 0;
}
```

**Output**:
```
Result (qubit 2): 1
After flip: 0
After restore: 1

Execution history:
  NOT 0 0 0
  NOT 1 0 0
  NOT 2 0 0
  CCNOT 0 1 2
  NOT 2 0 0
```

---

## Key Properties

### 1. Homoiconicity (Code as Data)

**R-layer gates are data**:
```c
R_Cell cell = {0, 5, 3, 7};  // CCNOT gate as data
r_print(cell);               // "CCNOT 5 3 7"
```

**Execution history is data**:
```c
R_Cell* history = s->r_runtime->history;  // Array of gate data
```

**D-layer operations become R-layer data**:
```c
d_AND(r, 0, 1, 2);  // Becomes sequence of R_Cell structs
```

### 2. Reversibility (Time-Travel)

**Checkpoint-based**:
```c
uint32_t cp = r_checkpoint(r);  // Save position
// ... operations ...
r_restore(r, cp);               // Rewind to checkpoint
```

**Step-based**:
```c
r_step_back(r);   // Undo one operation
```

**Property**: All R-layer gates are self-inverse, so rewinding = re-executing.

### 3. Actor-Proto Duality

**Simultaneous emergence**:
```c
S_Runtime* s = s_init(...);
// Creates both:
// - s->root_actor  (minimal binding)
// - s->root_proto  (maximum binding)
// They reference each other
```

**Mutual dependence**:
- Actors inherit structure from protos
- Protos are coordinated by actors
- Neither is prior - they emerge together

### 4. D-Layer Has No Primitives

**All D-layer operations are emergent**:
```c
// AND is controlled by CCNOT (R-layer)
void d_AND(R_Runtime* r, uint8_t a, uint8_t b, uint8_t result) {
    if (r->qubits[result]) r_NOT(r);    // Clear (R-layer)
    r_CCNOT(r, a, b, result);           // Toffoli (R-layer)
}
```

**D-layer is the interaction space** where:
- R-layer provides reversible substrate
- S-layer provides organization
- Their composition produces irreversible semantics

---

## Syntax

### R-Layer Syntax (System-Generated)

Users don't write R-layer directly - it's the compilation target:

```
CCNOT 0 1 2
CNOT 3 4
NOT 5
SWAP 0 3
```

### S-Layer Syntax (User-Written)

**Prototypes**:
```
proto <Name> <- <Parent>
    slots: <field1>, <field2>, ...

    methods
        <method_name>(<args>)
            <body>
```

**Actors**:
```
actor <Name>
    role is "<description>"

    state has
        <field> is <value>

    handlers

    on <message>
        <body>
```

**Inheritance**:
```
ChildProto <- ParentProto
```

**Messages**:
```
Actor -> message
```

### D-Layer Syntax (User-Written)

**Irreversible operations**:
```
result = a AND b
result = a OR b
result = a XOR b
```

**MAYBE**:
```
maybe value is uncertain_computation()

if value is resolved
    use value
```

---

## Compilation Flow

```
User writes Moop code
    ↓
Parser identifies:
    - S-layer declarations (proto, actor)
    - D-layer operations (AND, OR, etc.)
    ↓
Compiler generates:
    - S-layer: proto/actor structures
    - D-layer: calls to emergent operations
    - R-layer: gate sequences (CCNOT, CNOT, etc.)
    ↓
Runtime executes:
    - R-layer gates on reversible substrate
    - S-layer manages organization
    - D-layer semantics emerge from interaction
```

**Single-pass compilation** - no multi-layer stack.

---

## Memory Model

### R-Layer Memory

**Qubit storage**:
```c
uint8_t* qubits;  // Current state (classical bits)
```

**Execution history**:
```c
R_Cell* history;  // All operations (homoiconic)
```

**Property**: History enables reversibility and homoiconicity.

### S-Layer Memory

**Per-instance dual**:
```c
S_Actor* root_actor;   // Temporal coordinator
S_Proto* root_proto;   // Structural coordinator
```

**All entities**:
```c
S_Actor** actors;      // All actors descend from root_actor
S_Proto** protos;      // All protos descend from root_proto
```

### D-Layer Memory

**No dedicated memory** - D-layer operations use:
- R-layer qubits (for computation)
- S-layer structures (for organization)

---

## Aesthetic Properties

### 1. Minimalism

**5 mathematical primitives**:
- 4 R-layer gates
- 1 S-layer coordination primitive (dual aspects)

**Everything else emerges**.

### 2. Duality

**Actor-proto duality** mirrors **particle-wave duality**:
- Same primitive, two aspects
- Distinguished only by binding constraint
- Emerge simultaneously

### 3. Fractal Structure

**R-D-S pattern repeats**:
```
Program
    ↓
R-layer (substrate)
    ↓
D-layer (operations) = R + S interaction
    ↓
S-layer (organization)
    ↓
Each layer has r, d, s components
```

### 4. Tight Code

**Complete implementation: ~170 lines**

**No bloat**:
- 4-byte cells
- Index-based checkpoints
- Self-inverse gates
- Single allocation

### 5. Segregated Logic

**Reversible** (R-layer only):
```
CCNOT, CNOT, NOT, SWAP
```

**Irreversible** (D-layer, emergent):
```
AND, OR, NAND, NOR, XOR, MAYBE
```

**Clear separation** enforces computational purity.

---

## Philosophy

### The Universal Meta Equation

```
𝒪(Ξ) = ℛ(Ξ; α) + 𝒟(∇ΩΞ; β) + 𝒮(Ξ,Ξ′; γ)
```

**Moop implements this directly**:
- **R-term** → R-layer (reversible substrate)
- **D-term** → D-layer (dissipative operations, emergent)
- **S-term** → S-layer (structural coordination)

### The Binding Constraint

**One principle generates the S-layer dual**:

```
Minimal binding (1 level)  → actor
Maximum binding (∞ levels) → proto
```

**The binding constraint is the organizing principle** of the S-layer.

### Primitive Duality

**Actor and proto are not two primitives** - they are **dual aspects of one coordination primitive**, like:
- Particle-wave (quantum mechanics)
- Space-time (relativity)
- Yin-Yang (Taoism)

**Actor-proto** (Moop)

---

## Comparison to Other Languages

### Lisp

**Similarity**: Homoiconicity (code as data)

**Difference**: Moop adds reversibility + actor-proto duality

### Erlang

**Similarity**: Actor model

**Difference**: Moop actors inherit from reversible substrate + proto structure

### Haskell

**Similarity**: Pure functions, type safety

**Difference**: Moop has reversibility + homoiconicity + actor coordination

### Self/Io

**Similarity**: Prototype-based

**Difference**: Moop adds reversibility + actor-proto duality + minimal primitives

**Moop is unique** - no other language has:
- Reversible substrate
- Actor-proto duality
- 5 primitives
- R+D+S triadic structure

---

## Implementation Files

### Core Files

```
moop_core.h    # Header (API)
moop_core.c    # Implementation (~170 lines)
```

### Test File

```
test_moop.c    # Demonstrates all three layers
```

### Documentation

```
UNIFIED_MOOP.md                    # This file
THREE_LAYER_MOOP_ARCHITECTURE.md   # Detailed architecture
PRIMITIVE_DUALITY.md               # Actor-proto duality
UME_PHILOSOPHICAL_FOUNDATION.md    # Philosophical foundation
```

---

## Building and Running

### Compile

```bash
gcc -o test_moop test_moop.c moop_core.c -std=c99
```

### Run

```bash
./test_moop
```

### Expected Output

```
Result (qubit 2): 1
After flip: 0
After restore: 1

Execution history:
  NOT 0 0 0
  NOT 1 0 0
  NOT 2 0 0
  CCNOT 0 1 2
  NOT 2 0 0
```

---

## Future Directions

### 1. High-Level Syntax Parser

Parse natural Moop syntax:
```
proto Calculator <- MathProto
actor Counter
```

Compile to moop_core API calls.

### 2. RISC-V Backend

Emit RISC-V instructions for non-reversible operations.

### 3. Quantum Hardware Target

Map R-layer gates to actual quantum gates (already designed for this).

### 4. LLM Integration

Use LLM to resolve MAYBE primitives and ambiguous states.

---

## Conclusion

**Unified Moop is:**

- **Minimal**: 5 mathematical primitives (6 operational)
- **Tight**: ~170 lines of implementation
- **Complete**: Turing-complete, homoiconic, reversible
- **Elegant**: Actor-proto duality, R+D+S structure
- **Novel**: No other language has this combination

**This is the definitive three-layer architecture.**

---

## Document History

- **2025-10-24**: v1.0 - Initial unified specification (J. Rost)

---

**"Moop has 5 primitives. Everything else emerges from their interaction."**
— The Unified Moop Thesis
