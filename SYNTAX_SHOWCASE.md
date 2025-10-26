# Moop Syntax Showcase

**Code that reads like structured English • Quorum's naturalism + Io's minimalism**

This showcases Moop's actual syntax with real examples from the language specification.

---

## Core Design Principles

### Pythonic Whitespace
Moop uses **indentation for structure** like Python - no curly braces, no semicolons:

```moop
actor Calculator
    state has                  # Indentation shows structure
        history is []
        last_result is 0

    handlers
        on compute(x, y)       # Deeper indentation = nested scope
            result <-> x + y   # Even deeper = method body
            return result
```

**Whitespace is meaningful.** Clean visual hierarchy, no punctuation noise.

### The Three Arrows

Moop uses **three arrows** for different semantic layers:

```moop
# <- Structural (S-layer): Inheritance, binding
ChildProto <- ParentProto
variable <- value

# -> Dissipative (D-layer): Message passing, irreversible operations
actor -> message(data)
output -> "Hello"

# <-> Reversible (R-layer): Bidirectional computation, can be undone
result <-> reversible_operation(input)
answer <-> calculate(42 + 17)
```

**Three arrows. Three layers. Complete semantic clarity.**

---

## Example 1: Proto with Methods

```moop
proto MathProto <- ObjectProto
    slots: precision

    methods
        add(a, b)
            result <-> a + b      # Reversible computation
            return result

        factorial(n)
            if n <= 1
                return 1
            else
                prev <-> self.factorial(n - 1)
                result <-> n * prev
                return result
```

**Notice:**
- `<-` for inheritance
- `<->` for reversible mathematical operations
- Natural `slots:` declaration
- Methods read like functions
- No semicolons, minimal punctuation
- Pythonic indentation

---

## Example 2: Actor with State and Handlers

```moop
actor Calculator
    role is "Perform calculations with history"

    state has
        history is []
        last_result is 0

    handlers

    on compute(operation, args)
        maybe result is MathProto.call(operation, args)

        when result is defined:
            state.last_result = result
            state.history.append({op: operation, result: result})
            output -> "Result: " + result
        otherwise:
            output -> "Computation failed"

    on undo
        if history.length > 0
            state.history.pop()
            restore_checkpoint(history.length)
```

**Notice:**
- `role is` describes purpose in English
- `state has` declares state naturally
- `on <message>` for message handlers
- `maybe` for uncertain computation
- `when ... is defined:` / `otherwise:` for MAYBE logic
- `->` for output and messages

---

## Example 3: Message Passing

```moop
# Create actor
calc <- Calculator.spawn()

# Send messages with ->
calc -> compute("factorial", [5])
calc -> compute("add", [10, 20])
calc -> undo
```

**Notice:**
- `<-` for assignment/binding
- `->` for sending messages
- Natural method call syntax

---

## Example 4: Proto Inheritance Chain

```moop
proto Point <- ObjectProto
    slots: x, y

    methods
        distance_to(other)
            dx = self.x - other.x
            dy = self.y - other.y
            return sqrt(dx*dx + dy*dy)

proto Point3D <- Point
    slots: z    # Inherits x, y from Point

    methods
        distance_to(other)
            dx = self.x - other.x
            dy = self.y - other.y
            dz = self.z - other.z
            return sqrt(dx*dx + dy*dy + dz*dz)
```

**Notice:**
- Clean inheritance with `<-`
- Method override just works
- Inherits slots automatically

---

## Example 5: Trinary MAYBE Logic

```moop
maybe user_authenticated is check_credentials(username, password)

when user_authenticated is true:
    session <- create_session(user)
    output -> "Login successful"

when user_authenticated is false:
    output -> "Login failed"

otherwise:
    # Still uncertain (e.g., waiting for 2FA)
    output -> "Awaiting multi-factor authentication"
```

**Notice:**
- `maybe` for uncertain values
- `when ... is true:` / `is false:` / `otherwise:`
- Natural English phrasing

---

## Example 6: Reversible Computation (<->)

The `<->` arrow enables **bidirectional computation** - operations that can be undone:

```moop
# Mathematical operations are reversible
result <-> calculate(42 + 17)
# result = 59, and the system knows 59 came from 42 + 17

# Can decompose back
components <-> decompose(result)
# components = [42, 17]

# Complex mathematical transformations
encrypted <-> encrypt(message, key)
# Later, can reverse it
message <-> decrypt(encrypted, key)

# Reversible actor operations
actor Calculator
    state has
        history is []

    handlers

    on compute(operation, args)
        result <-> MathProto.call(operation, args)  # Reversible
        state.history.append({op: operation, result: result})
        output -> "Result: " + result               # Irreversible I/O

    on undo
        if history.length > 0
            state.history.pop()
            # R-layer automatically maintains reversibility
            restore_checkpoint(history.length)
```

**The power of `<->`:**
- **Mathematical reversibility**: `42 + 17 <-> 59` maintains the relationship
- **Undo/redo**: System tracks how values were computed
- **Decomposition**: Can reconstruct inputs from outputs
- **Debugging**: Can rewind computation to any point

**Contrast with `->`:**
- `output -> "Hello"` is **irreversible** (can't "un-print")
- `result <-> 2 + 2` is **reversible** (system knows result came from 2+2)

---

## The Synergistic Power: Homoiconicity + Reversibility

Moop combines two foundational properties that create something **greater than the sum of their parts**:

### 1. Homoiconicity (Code is Data)
```moop
# Read operation from tape
operation <- tape.read(index)
# operation = {gate: CNOT, target_a: 0, target_b: 1}
```

### 2. Reversibility (Operations Can Be Undone)
```moop
# Execute operation
result <-> execute(operation)
# Can rewind back to before execution
```

### 3. The Synergy: **Self-Modifying Reversible Code**

When you combine them, you get unprecedented power:

```moop
# The system can modify its own code...
operation <- tape.read(100)
operation.gate = SWAP        # Change the operation
tape.write(100, operation)   # Update the code

# ...execute the modified code...
result <-> execute_tape(100)

# ...and REWIND through the modification!
tape.undo()                  # Reverses to original operation
result.undo()                # Reverses the execution

# The system can explore alternative versions of itself
# and backtrack when needed
```

### What This Enables

**Traditional homoiconicity (Lisp, Forth):**
- ✓ Code can modify itself
- ✗ Can't undo modifications
- ✗ Can't explore alternative code paths safely

**Traditional reversibility (quantum computing):**
- ✓ Operations can be undone
- ✗ Code structure is fixed
- ✗ Can't modify the algorithm while running

**Moop's homoiconicity + reversibility:**
- ✓ Code can modify itself
- ✓ Modifications can be undone
- ✓ **Explore code space reversibly**
- ✓ **Self-evolving programs that can backtrack**
- ✓ **Meta-programming with undo**

### Real Application: Evolutionary Self-Optimization

```moop
actor SelfOptimizer
    state has
        best_performance is 0
        best_code is none

    handlers

    on optimize_self
        # Save current state
        checkpoint <- tape.checkpoint()

        # Try modification
        current_code <- tape.read(self.algorithm_index)
        current_code.gate = alternative_gate
        tape.write(self.algorithm_index, current_code)

        # Test performance (reversibly)
        performance <-> measure_performance()

        if performance > state.best_performance
            # Keep this version
            state.best_performance = performance
            state.best_code = current_code
        else
            # Revert to previous version
            tape.restore(checkpoint)
```

**The synergy creates:**
1. **Exploratory programming** - Try code changes and revert if they don't work
2. **Self-evolving systems** - Programs that optimize themselves
3. **Safe meta-programming** - Modify code structure with a safety net
4. **Temporal debugging** - Step backwards through code modifications
5. **Evolutionary algorithms** - The code itself can evolve and backtrack

**This is unique to Moop.** No other language combines these properties at the foundational level.

---

## Structured as a Complex Adaptive System

Moop isn't just a programming language - it's designed as a **complex adaptive system** following Prigogine's dissipative structures:

### What This Means

**Traditional programming languages:**
- Static, deterministic semantics
- Fixed optimization strategies
- No self-organization
- Manual evolution required

**Moop as a Complex Adaptive System:**
- **Self-organizing** - Code naturally organizes based on fitness
- **Emergent behavior** - System-level patterns emerge from simple rules
- **Adaptive** - Responds to computational pressures
- **Far-from-equilibrium** - Maintains dynamic stability through energy flow
- **Dissipative** - Exports entropy (irreversible operations) while maintaining order

### The Three Layers as Energy Flows

```
R-layer (Reversible)     ← Energy conserving, information preserving
    ↓
D-layer (Dissipative)    ← Energy dissipating, entropy exporting
    ↓
S-layer (Structural)     ← Pattern organizing, self-maintaining
```

**Energy flows through the system**, creating order from chaos.

### Real Benefits for Programmers

#### 1. Evolutionary Code Optimization
```moop
# The system naturally selects better code
# No manual optimization needed
actor Calculator
    # Frequently used operations survive
    # Rarely used operations get pruned
    # Fitness = f(recency, activity, gate_type)
```

**Benefit:** Code optimizes itself based on actual usage patterns.

#### 2. Self-Organizing Memory
```moop
# The tape-loop maintains the most useful operations
# Automatically prunes low-fitness code
# Creates emergent execution patterns
```

**Benefit:** Memory management becomes an emergent property, not a manual task.

#### 3. Adaptive Performance
```moop
# System tunes its own fitness parameters
fitness_params.recency_weight = 0.7  # Adapts to workload
system.tune_fitness(fitness_params)
```

**Benefit:** Performance characteristics evolve to match your computational patterns.

#### 4. Emergent Stability
```moop
# High-fitness operations become "essential"
# System maintains critical paths automatically
# Natural resistance to catastrophic failure
```

**Benefit:** Robustness emerges from selection pressure, not defensive programming.

### The Power of Far-From-Equilibrium Dynamics

Traditional systems seek equilibrium (stable, boring, dead).

**Moop maintains far-from-equilibrium dynamics:**
- Constant energy flow (computation)
- Entropy export (irreversible operations to D-layer)
- Order maintenance (structural patterns in S-layer)
- Self-organization (evolutionary pruning)

**Result:** Living, adaptive code that responds to its environment.

### Complex Adaptive Benefits Summary

| Property | Traditional Languages | Moop (CAS) |
|----------|----------------------|------------|
| Optimization | Manual profiling | Emergent from fitness |
| Memory management | Manual or GC | Self-organizing tape |
| Performance tuning | Fixed heuristics | Adaptive parameters |
| Code organization | Manual refactoring | Evolutionary selection |
| Stability | Defensive programming | Emergent from selection |
| Evolution | External tools | Built-in meta-evolution |

**Moop treats your program as a living system**, not a static artifact.

### The Synergy with Homoiconicity + Reversibility

When you combine:
1. **Complex Adaptive System** structure
2. **Homoiconicity** (code is data)
3. **Reversibility** (operations can be undone)

You get: **Self-evolving systems that explore code space safely while maintaining emergent stability.**

```moop
# The system explores alternative code structures
checkpoint <- tape.checkpoint()

# Try evolutionary modification
mutate_code(fitness_criteria)

# Test performance (reversibly)
performance <-> measure_fitness()

if performance < threshold
    # Revert to checkpoint
    tape.restore(checkpoint)
else
    # Keep mutation, it's better!
    mark_essential(current_state)
```

**Programs become organisms** that adapt, evolve, and optimize themselves.

---

## Perfect for Embedded Systems

Moop combines **gate-based computational substrate** with **lean conventional memory** for an ideal embedded profile:

### Dual Memory Architecture

**System layer (L1)**: Gate-based tape-loop
- 1024 circular cells (fixed size)
- Reversible gate operations (CCNOT, CNOT, NOT, SWAP)
- Evolutionary pruning (automatic cleanup)
- No dynamic allocation in computational substrate

**User layer (L3b)**: Conventional memory for actor state and proto slots
- Managed by lean C runtime
- No garbage collector
- Predictable allocation patterns

### Why Moop Excels in Embedded Environments

✅ **Small footprint** - ~40KB total runtime (tape + qubit state + runtime)
✅ **No garbage collector** - No unpredictable GC pauses
✅ **Deterministic behavior** - Pruning every 256 ops, O(1) fitness computation
✅ **Fixed computational memory** - 1024-cell tape never grows
✅ **Lean C implementation** - ~680 lines, compiles to bare metal
✅ **Self-managing substrate** - Evolutionary pruning handles tape cleanup automatically

```moop
actor SensorController
    state has
        readings is []           # User memory: conventional allocation
        last_value is 0

    handlers
    on process_sensor_data(value)
        # Computation happens on L1 gate-based tape
        result <-> analyze(value)

        # State updates use conventional memory
        state.readings.append(result)
        state.last_value = result
```

### Ideal Use Cases

- Embedded systems (IoT, microcontrollers, sensors)
- Real-time systems (no GC pauses, deterministic timing)
- Resource-constrained environments (fixed computational memory)
- Safety-critical applications (predictable behavior, no hidden allocations)

### Comparison

| Language | Runtime Size | GC Pauses | Deterministic | Computational Memory |
|----------|--------------|-----------|---------------|----------------------|
| Python | ~15MB | Yes | No | Heap (unbounded) |
| Java | ~50MB | Yes | No | Heap (unbounded) |
| Go | ~2MB | Yes | No | Heap (unbounded) |
| Rust | ~500KB | No | Yes | Heap (manual) |
| C | Variable | No | Yes* | Heap (malloc) |
| Forth | ~10KB | No | Yes | Stack (fixed) |
| **Moop** | **~40KB** | **No** | **Yes** | **Tape (1024 cells, self-managing)** |

*C is deterministic only if you avoid malloc/free

**Key advantage:** Moop's computational substrate (L1 tape-loop) is **self-managing** through evolutionary pruning, while user memory remains simple and predictable.

---

## Example 7: Natural Language Actor Definition

From the test suite - this is what actual Moop code looks like:

```moop
actor UserManager
    role is "handles user authentication and sessions"
    state has
        logged_in is false
```

**And proto definitions:**

```moop
proto DatabaseConnection <- Object
    slots: host, port, timeout
```

**That's real Moop code!** No curly braces, no semicolons, just English.

---

## Example 7: Complex Actor with MAYBE

```moop
actor AuthenticationService
    role is "Manages user authentication with confidence scoring"

    state has
        active_sessions is {}
        failed_attempts is 0

    handlers

    on authenticate(username, password, mfa_code)
        # Check credentials
        maybe credentials_valid is verify_password(username, password)
        maybe mfa_valid is verify_mfa(mfa_code)

        # Combine confidence scores
        when credentials_valid is true and mfa_valid is true:
            session <- create_session(username)
            active_sessions[username] = session
            failed_attempts = 0
            output -> "Authentication successful"

        when credentials_valid is false:
            failed_attempts = failed_attempts + 1
            output -> "Invalid credentials"

        otherwise:
            output -> "Awaiting verification"

    on logout(username)
        if username in active_sessions:
            destroy_session(active_sessions[username])
            delete active_sessions[username]
            output -> "Logged out successfully"
```

**Notice:**
- Multiple `maybe` values
- Combined conditions with `and`
- Dictionary/map syntax `{}`
- Natural control flow

---

## Example 8: Self-Modifying Code

```moop
# Read the tape (code is data)
operation <- tape.read(index)

# Modify it
operation.gate = NOT
operation.target = 5

# Write it back (data is code)
tape.write(index, operation)

# The system has modified its own behavior
```

**Homoiconicity:** Code and data are the same thing.

---

## Example 9: Meta-Evolution

```moop
# The system can tune how it evolves
fitness_params <- system.get_fitness_params()

fitness_params.recency_weight = 0.7
fitness_params.activity_weight = 0.2
fitness_params.gate_weight = 0.1

system.tune_fitness(fitness_params)

# Now the evolutionary pressure has changed
```

**Meta-evolution:** The system evolves how it evolves.

---

## Comparison: Same Logic, Different Languages

### Python
```python
class Calculator:
    def __init__(self):
        self.history = []
        self.last_result = 0

    def compute(self, operation, args):
        result = getattr(self, operation)(*args)
        self.last_result = result
        self.history.append({'op': operation, 'result': result})
        print(f"Result: {result}")
```

### JavaScript
```javascript
class Calculator {
    constructor() {
        this.history = [];
        this.lastResult = 0;
    }

    compute(operation, args) {
        const result = this[operation](...args);
        this.lastResult = result;
        this.history.push({op: operation, result: result});
        console.log(`Result: ${result}`);
    }
}
```

### Moop
```moop
actor Calculator
    role is "Perform calculations with history"

    state has
        history is []
        last_result is 0

    handlers

    on compute(operation, args)
        result = self.call(operation, args)
        state.last_result = result
        state.history.append({op: operation, result: result})
        output -> "Result: " + result
```

**See the difference?**
- No `class`, `constructor`, `__init__` ceremony
- No `self` parameter declarations
- Natural English phrases: `role is`, `state has`, `on`
- Message handlers instead of methods
- Reads like a specification

---

## Key Syntax Elements

### The Three Arrows (Core Semantics)

| Syntax | Meaning | Layer | Properties |
|--------|---------|-------|------------|
| `<-` | Inheritance, binding, assignment | S-layer | Structural relationships |
| `->` | Message passing, irreversible operations | D-layer | One-way, can't undo |
| `<->` | Reversible computation, bidirectional | R-layer | Can be undone/inverted |

### Language Constructs

| Syntax | Meaning | Layer |
|--------|---------|-------|
| `proto ... <- ...` | Prototype definition | S-layer |
| `actor ...` | Actor definition | S-layer |
| `role is` | Actor purpose (English) | S-layer |
| `state has` | State declaration (English) | S-layer |
| `slots:` | Proto slots | S-layer |
| `methods` | Method definitions | S-layer |
| `handlers` | Message handlers | S-layer |
| `on <message>` | Handle message | D-layer |
| `maybe` | Uncertain value | D-layer |
| `when ... is defined:` | MAYBE resolved | D-layer |
| `otherwise:` | MAYBE fallback | D-layer |
| `output ->` | Side effect (irreversible) | D-layer |
| `is`, `has` | Natural linking verbs | All |
| Indentation | Block structure (Pythonic) | All |

---

## Philosophy

**Quorum's naturalism:**
- `role is "description"`
- `state has field is value`
- `when condition is true:`
- Natural language verbs and structure

**Io's minimalism:**
- Everything is message passing
- Prototype-based (no classes)
- Three arrows: `<-`, `->`, `<->`
- No ceremony, no boilerplate

**Python's elegance:**
- Indentation for structure
- No curly braces, no semicolons
- Whitespace is meaningful
- Visual clarity through spacing

**The result:**
- Code that reads like structured English
- Pythonic whitespace discipline
- Three arrows capture all semantics
- Minimal punctuation, maximum expressiveness
- Elegant, powerful, and reversible

---

## Try It

```bash
git clone https://github.com/Blobfish108/Mark_Rosst.git
cd unified_moop
make test
```

See `examples/` for working code and `tests/test_enhanced.c` for the natural language parser in action.

---

**Unified Moop v0.1.0-alpha** - Code that reads like English.

🦎 *Evolutionary code that evolves its own evolution*
