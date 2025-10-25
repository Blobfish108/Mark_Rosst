# Three-Layer Moop: Clarified Architecture

**Author**: Joseph Rost (Philosopher)
**Date**: October 2025
**Status**: Definitive Specification

---

## The Three Layers

### R-Layer: Reversible Substrate (System-Facing)

**R-layer is the foundation** - everything else is hosted on it.

**Properties**:
1. **Turing-complete** - Can express all computation
2. **Homoiconic** - Gates are data (HRIR cells)
3. **Reversible** - All operations can be undone
4. **System-facing** - Users don't write R-layer code directly

**Primitives** (4 reversible gates):
```
CCNOT (Toffoli) - Universal reversible gate
CNOT            - Controlled NOT
NOT             - Bit flip
SWAP            - Exchange qubits/bits
```

**Example** (system-generated, not user-written):
```
CCNOT q0 q1 q2
CNOT q3 q4
SWAP q0 q2
NOT q1
```

**Key insight**: The R-layer is **the substrate**. Everything above **inherits homoiconicity and reversibility** from this foundation.

---

### D-Layer: Dissipative Operations (User-Facing)

**D-layer is hosted on R-layer** but provides **irreversible** operations for users.

**Purpose**: Allow information-destroying operations (entropy creation).

**Operations** (NOT primitives - controlled by primitives):
```
AND             - Controlled by CCNOT (R-layer) and actor/proto (S-layer)
OR              - Controlled by CCNOT + NOT (R-layer) and actor/proto (S-layer)
NAND            - Controlled by CCNOT + NOT (R-layer) and actor/proto (S-layer)
NOR             - Controlled by CCNOT + NOT (R-layer) and actor/proto (S-layer)
XOR             - Controlled by CNOT (R-layer) and actor/proto (S-layer)
MAYBE           - Controlled by actor/proto (S-layer), data structure on R-layer
```

**Key insight**: D-layer operations are **emergent** - they are controlled by the 6 primitives (4 R-layer gates + 2 S-layer constructs) but have **irreversible semantics** - information is destroyed.

**Example**:
```moop
// User writes this (D-layer):
let result = a AND b

// Compiles to R-layer gates:
// CCNOT + ancilla bits to implement irreversible AND
// But the semantic is: information lost (a=1,b=0 and a=0,b=0 both give result=0)
```

**MAYBE primitive**:
```moop
// Superposition of uncertain states
maybe user_authenticated is check_credentials()

// Computation continues with unresolved state
if user_authenticated
    process_secure_operation()
else
    show_login_screen()

// Collapse happens at observation point
```

**Actors** (D-layer coordination):
```moop
actor Counter
    role is "Count events"

    state has
        count is 0

    handlers

    on increment
        state.count = count + 1  // Uses D-layer operations

    on reset
        state.count = 0
```

**Actors use D-layer operations** but are **hosted on R-layer substrate**, so they inherit:
- **Homoiconicity** (actor state is data)
- **Reversibility** (actor operations can be checkpointed and undone via R-layer)

---

### S-Layer: Structural Organization (User-Facing)

**S-layer provides structure** via prototypes and inheritance.

**Key principle**: **Each program instance has its own `root_proto`**.

**Root proto per program**:
```moop
// Program instance 1
root_proto_1
    ↓
ObjectProto_1 <- root_proto_1
    ↓
MathProto_1 <- ObjectProto_1

// Program instance 2 (separate root!)
root_proto_2
    ↓
ObjectProto_2 <- root_proto_2
    ↓
Calculator_2 <- ObjectProto_2
```

**No global root_proto** - each program instance gets isolation.

**Prototype syntax**:
```moop
proto Calculator <- MathProto
    slots: value, history

    methods
        add(x)
            value = value + x           // D-layer operation
            history.append("add " + x)  // D-layer operation

        reset()
            value = 0
```

**Inheritance chain**:
```moop
Calculator <- MathProto <- ObjectProto <- root_proto
```

**Every object carries ancestry** from its program instance's `root_proto`.

---

## How the Layers Interact

### Inheritance of Properties

```
R-layer (substrate)
    ↓ provides homoiconicity
    ↓ provides reversibility
    ↓
D-layer (operations)
    ↓ uses R-layer substrate
    ↓ adds irreversible semantics
    ↓ inherits homoiconicity
    ↓ inherits reversibility (via checkpoints)
    ↓
S-layer (structure)
    ↓ uses D-layer operations
    ↓ adds inheritance
    ↓ inherits homoiconicity
    ↓ inherits reversibility
```

**Key**: Everything inherits from R-layer's foundational properties.

### Compilation Flow

```
User writes:
    S-layer: proto Calculator <- MathProto
    D-layer: Counter -> increment
    D-layer: result = a AND b

Compiler:
    1. Parse S-layer declarations → build prototype graph
    2. Parse D-layer operations → identify irreversible gates
    3. Compile D-layer gates → R-layer reversible gates + ancilla
    4. Emit R-layer code → CCNOT, CNOT, NOT, SWAP

Runtime:
    - R-layer executes reversible gates
    - D-layer semantics emerge from R-layer implementation
    - S-layer structure organizes the process
```

---

## The Six Primitives (Total)

### R-Layer: 4 Reversible Gates (System-Facing)

```
1. CCNOT (Toffoli)
2. CNOT
3. NOT
4. SWAP
```

**Properties**:
- Turing-complete
- Homoiconic (gates as data)
- Reversible (can undo)
- **These are the ONLY primitives at the substrate level**

### S-Layer: 2 Organizational Primitives (User-Facing)

```
5. actor  (minimal binding)
6. proto  (maximum binding)
```

**Properties**:
- Structural organization
- Hosted on R-layer
- Inherit homoiconicity and reversibility

---

## D-Layer Operations (NOT Primitives)

**D-layer provides emergent operations** that are **controlled by the 6 primitives**:

```
AND   → controlled by CCNOT (R-layer) + actor/proto (S-layer)
OR    → controlled by CCNOT + NOT (R-layer) + actor/proto (S-layer)
NAND  → controlled by CCNOT + NOT (R-layer) + actor/proto (S-layer)
NOR   → controlled by CCNOT + NOT (R-layer) + actor/proto (S-layer)
XOR   → controlled by CNOT (R-layer) + actor/proto (S-layer)
MAYBE → controlled by actor/proto (S-layer), homoiconic data on R-layer
```

**These are NOT primitives** - they **emerge from** the composition of reversible gates (R-layer) and organizational constructs (S-layer).

---

## Total Primitive Count: 6

**System-facing (R-layer)**: 4 gates (CCNOT, CNOT, NOT, SWAP)
**User-facing (S-layer)**: 2 constructs (actor, proto)

**Total**: **6 primitives**

**Everything else is emergent** (controlled by the primitives):
- D-layer operations (AND, OR, etc.) → controlled by reversible gates + actor/proto
- Functions → controlled by reversible gates + actor/proto
- Messages → controlled by actor (S-layer) + reversible gates (R-layer)
- I/O → controlled by actor/proto (S-layer) + reversible gates (R-layer)

**The D-layer has no primitives** - it is the **coordination space** where R-layer gates and S-layer organization interact to produce emergent irreversible semantics.

---

## Segregated Logic (Revisited)

### R-layer: Reversible Gates Only

```
CCNOT, CNOT, NOT, SWAP
```

**Property**: Information-preserving, time-symmetric.

### D-layer: Irreversible Gates Only

```
AND, OR, NAND, NOR, XOR, MAYBE
```

**Property**: Information-destroying, entropy-creating.

**Key insight**: Reversible and irreversible gates are **segregated by layer**.

**This enforces computational purity.**

---

## Homoiconicity at Every Layer

### R-layer Homoiconicity

**Gates are data**:
```c
typedef struct HRIR_Cell {
    uint32_t id;
    const char* opcode;       // "CCNOT", "CNOT", etc.
    const char** args;        // ["q0", "q1", "q2"]

    struct HRIR_Cell* inverse;  // Inverse operation
    bool is_reversible;         // Always true for R-layer

    const char* canonical_path; // Proto.Actor.Func
} HRIR_Cell;
```

**Code is data, data is code.**

### D-layer Homoiconicity

**Operations inherit gate representation**:
```moop
// User writes:
result = a AND b

// Becomes HRIR cell:
{
    opcode: "AND",
    args: ["a", "b", "result"],
    inverse: NULL,             // AND is irreversible
    is_reversible: false,
    canonical_path: "UserProto.Compute.AND"
}
```

**D-layer operations are data** (inherited from R-layer).

### S-layer Homoiconicity

**Prototypes are data**:
```moop
proto Calculator <- MathProto
    slots: value
    methods: add, subtract

// Becomes data structure:
{
    name: "Calculator",
    parent: &MathProto,
    slots: ["value"],
    methods: {
        "add": HRIR_Cell*,
        "subtract": HRIR_Cell*
    }
}
```

**Structure is data** (inherited from R-layer).

---

## Reversibility at Every Layer

### R-layer Reversibility

**All gates have inverses**:
```
CCNOT is self-inverse (CCNOT² = I)
CNOT is self-inverse (CNOT² = I)
NOT is self-inverse (NOT² = I)
SWAP is self-inverse (SWAP² = I)
```

**Perfect reversibility at substrate.**

### D-layer Reversibility (via Checkpoints)

**Irreversible operations can be undone via R-layer checkpoints**:
```moop
// Save checkpoint (R-layer)
checkpoint = save_state()

// Perform irreversible operation (D-layer)
result = a AND b

// Rewind if needed (R-layer)
restore(checkpoint)
```

**D-layer inherits reversibility** from R-layer substrate, even though individual D-layer gates are irreversible.

### S-layer Reversibility (via Prototype History)

**Prototype modifications can be undone**:
```moop
// Modify prototype
MathProto.add_method("new_func", implementation)

// Later, undo the modification
MathProto.remove_method("new_func")  // Restore previous state
```

**S-layer inherits reversibility** from R-layer.

---

## Root Proto Per Program Instance

### Isolation via Root Proto

**Traditional approach** (global root):
```
# BAD: All programs share one root_proto
root_proto
    ↓
Program1.ObjectProto
Program2.ObjectProto  # Collision!
```

**Moop approach** (per-instance root):
```
# GOOD: Each program instance has its own root
Program1:
    root_proto_1
        ↓
    ObjectProto_1
        ↓
    Calculator_1

Program2:
    root_proto_2
        ↓
    ObjectProto_2
        ↓
    BankAccount_2
```

**No global state** - each program instance is isolated.

### Root Proto Implementation

```c
typedef struct MoopProgramInstance {
    uint32_t instance_id;

    // Each instance gets its own root
    Proto* root_proto;

    // All prototypes descend from this root
    ProtoGraph* proto_graph;

    // Actor runtime
    ActorScheduler* actors;

    // Reversible gate memory
    ReversibleMemory* gates;
} MoopProgramInstance;
```

**Every program instance has**:
- Its own `root_proto`
- Its own prototype graph
- Its own actor scheduler
- Its own gate memory

**Complete isolation.**

---

## Example: Unified Moop Program

```moop
// S-layer: Structural organization
// (This program instance gets its own root_proto automatically)

proto MathProto <- ObjectProto  // ObjectProto <- root_proto (implicit)
    slots: precision

    methods
        add(a, b)
            // D-layer: Irreversible operation (destroys intermediate values)
            return a + b

        factorial(n)
            // D-layer: Recursive irreversible computation
            if n <= 1
                return 1
            else
                return n * (self.factorial(n - 1))

// D-layer: Actor coordination
actor Calculator
    role is "Perform calculations with history"

    state has
        history is []
        last_result is 0

    handlers

    on compute(operation, args)
        // D-layer: MAYBE for ambiguity
        maybe result is MathProto.call(operation, args)

        when result is defined:
            state.last_result = result
            state.history.append({op: operation, result: result})
            output -> "Result: " + result
        otherwise:
            output -> "Computation failed"

    on undo
        // Inherited reversibility from R-layer
        if history.length > 0
            state.history.pop()
            // Rewind R-layer state
            restore_checkpoint(history.length)

// Entry point
calc <- Calculator.spawn()              // S-layer: Instantiate actor
calc -> compute("factorial", [5])       // D-layer: Send message
calc -> compute("add", [10, 20])        // D-layer: Send message
calc -> undo                            // D-layer: Rewind via R-layer
```

**This program**:
- Uses **S-layer** for structure (MathProto, Calculator)
- Uses **D-layer** for operations (add, factorial, messages, MAYBE)
- Inherits **R-layer** properties (homoiconicity, reversibility)
- Gets its own **root_proto** instance

---

## The Three Arrows (Revisited)

### `<-` Structural Arrow (S-layer)

```moop
ChildProto <- ParentProto
```

**Meaning**: Inheritance, structural relationship.

### `->` Dissipative Arrow (D-layer)

```moop
Actor -> message
output -> "Hello"
```

**Meaning**: Message passing, irreversible operation.

### R-layer Operations (No Arrow Needed)

```moop
// R-layer operations are implicit
// Users don't write R-layer code directly
// Compiler generates R-layer gates automatically
```

**Meaning**: Users work at D/S layers, R-layer is the invisible substrate.

---

## Compilation Example

### User Code

```moop
proto Point <- ObjectProto
    slots: x, y

    methods
        distance_to(other)
            // D-layer: Uses irreversible gates
            let dx = self.x - other.x
            let dy = self.y - other.y
            return sqrt(dx*dx + dy*dy)

actor PointManager
    on create_point(x, y)
        let p = Point.new()
        p.x = x
        p.y = y
        output -> "Point created"
```

### Compilation Steps

**Step 1: Parse S-layer**
```
Proto: Point <- ObjectProto
    slots: [x, y]
    methods: [distance_to]

Actor: PointManager
    handlers: [create_point]
```

**Step 2: Parse D-layer operations**
```
Operations in distance_to:
    - subtract (irreversible)
    - multiply (irreversible)
    - sqrt (irreversible)

Operations in create_point:
    - Point.new() (allocation)
    - assignment (state mutation)
    - output (I/O, irreversible)
```

**Step 3: Compile D-layer to R-layer**
```
subtract → implemented via CCNOT + ancilla bits
multiply → implemented via CCNOT chains
sqrt → implemented via reversible Newton-Raphson
output → implemented via CNOT to I/O register
```

**Step 4: Emit R-layer gates**
```asm
# R-layer assembly (system-generated)
CCNOT q0 q1 q2
CNOT q3 q4
SWAP q1 q5
CCNOT q5 q6 q7
...
```

**Step 5: Link and execute**
```
R-layer gates execute on reversible substrate
D-layer semantics emerge from R-layer implementation
S-layer structure organizes the whole program
```

---

## Memory Model

### R-layer Memory (Reversible Gates)

**HRIR cells**:
- Store all computation history
- Checkpoints for reversibility
- Homoiconic representation

**Properties**:
- Immutable once written
- Forms DAG of computation history
- Enables time-travel debugging

### D-layer Memory (User State)

**Actor state, variables**:
- Mutable
- Uses D-layer operations (irreversible)
- Hosted on R-layer substrate

**Properties**:
- Fast access
- Can be checkpointed via R-layer
- Irreversible by default, but rewindable

### S-layer Memory (Prototype Graph)

**Prototype definitions**:
- Inheritance chains
- Method tables
- Slot layouts

**Properties**:
- Read-mostly (prototypes rarely change)
- Forms tree from root_proto
- Can be modified (homoiconic)

---

## Aesthetics of the Three-Layer Design

### 1. Clean Separation

**R-layer**: System-facing, reversible foundation
**D-layer**: User-facing, irreversible operations
**S-layer**: User-facing, structural organization

**No mixing** - each layer has a clear purpose.

### 2. Inheritance of Properties

**Homoiconicity**:
```
R-layer (gates are data)
    ↓
D-layer (operations are data)
    ↓
S-layer (structure is data)
```

**Reversibility**:
```
R-layer (gates have inverses)
    ↓
D-layer (checkpoints enable undo)
    ↓
S-layer (prototype changes can be undone)
```

**Properties flow downward** from substrate.

### 3. Segregated Logic

**Reversible gates** (R-layer):
```
CCNOT, CNOT, NOT, SWAP
```

**Irreversible gates** (D-layer):
```
AND, OR, NAND, NOR, XOR, MAYBE
```

**Never mixed** - enforces computational purity.

### 4. Root Proto Isolation

**Each program instance**:
```
root_proto_i
    ↓
user prototypes
```

**No global state** - programs are isolated.

### 5. Minimal Primitive Count

**System-facing**: 4 gates (R-layer)
**User-facing**: 7 gates + 2 constructs (D/S layers)

**Total**: 13 primitives, but users only see 9.

---

## Comparison to Original Layered Stack

### Original Stack (5 layers)

```
L5 Moop → L4 Rio → L3 Turchin → L2 Prigogine → L1 McCarthy
```

**Problems**:
- 5 different languages
- Complex compilation pipeline
- Unclear where properties come from

### Three-Layer Design

```
S-layer (structure: proto, actor)
    ↓ hosted on
D-layer (operations: AND, OR, XOR, MAYBE)
    ↓ hosted on
R-layer (substrate: CCNOT, CNOT, NOT, SWAP)
```

**Advantages**:
- 3 layers with clear roles
- Properties flow from R-layer
- Single unified language

---

## Conclusion

### The Definitive Three-Layer Architecture

**R-layer** (Reversible substrate):
- Turing-complete
- Homoiconic (gates as data)
- Reversible (all operations can be undone)
- System-facing (invisible to users)
- 4 primitives: CCNOT, CNOT, NOT, SWAP

**D-layer** (Dissipative operations):
- Hosted on R-layer
- Irreversible gates (information-destroying)
- User-facing
- Inherits homoiconicity and reversibility from R-layer
- 7 primitives: AND, OR, NAND, NOR, XOR, MAYBE, actor coordination

**S-layer** (Structural organization):
- Hosted on D-layer (which is hosted on R-layer)
- Prototypes with inheritance
- Each program instance has its own root_proto
- User-facing
- Inherits homoiconicity and reversibility from R-layer
- 2 primitives: actor, proto

**Total**: 13 primitives (4 system + 9 user)

**This is the true Moop architecture.**

---

## Next Steps

1. **Implement R-layer runtime** - Reversible gate execution engine
2. **Implement D-layer compiler** - Translate irreversible gates to R-layer
3. **Implement S-layer parser** - Prototype graph construction
4. **Per-instance root_proto** - Program isolation
5. **Unified syntax** - One language, three layers

---

**"The R-layer is the foundation. All else inherits from it."**
— The Three-Layer Moop Thesis
