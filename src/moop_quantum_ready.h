// moop_quantum_ready.h
// Quantum-Ready Abstraction Layer
// Default: Classical backend on conventional hardware
// Future: Drop-in quantum backend support

#ifndef MOOP_QUANTUM_READY_H
#define MOOP_QUANTUM_READY_H

#include <stdint.h>
#include <stdbool.h>

// ============================================================================
// Backend Types
// ============================================================================

typedef enum {
    QUBIT_BACKEND_CLASSICAL,    // Default: Classical bits (uint8_t)
    QUBIT_BACKEND_SIMULATOR,    // Future: Quantum simulator (statevector)
    QUBIT_BACKEND_QUANTUM       // Future: Real quantum hardware
} Qubit_Backend_Type;

// ============================================================================
// Abstract Qubit State
// ============================================================================

typedef struct Qubit_State {
    Qubit_Backend_Type backend_type;
    void* backend_data;         // Opaque pointer to implementation
    uint32_t qubit_count;

    // Backend-specific metadata
    void* metadata;             // For backend-specific info
} Qubit_State;

// ============================================================================
// Backend Operations (Virtual Function Table)
// ============================================================================

typedef struct {
    // Lifecycle
    Qubit_State* (*init)(uint32_t n_qubits);
    void (*free)(Qubit_State* state);
    Qubit_State* (*clone)(const Qubit_State* state);

    // Reversible gates (quantum-compatible)
    void (*CCNOT)(Qubit_State* state, uint8_t a, uint8_t b, uint8_t c);
    void (*CNOT)(Qubit_State* state, uint8_t a, uint8_t b);
    void (*NOT)(Qubit_State* state, uint8_t a);
    void (*SWAP)(Qubit_State* state, uint8_t a, uint8_t b);

    // Measurement (collapses quantum superposition to classical bit)
    uint8_t (*measure)(Qubit_State* state, uint8_t qubit);

    // Read state (for classical: direct read, for quantum: measure without collapse if possible)
    uint8_t (*read)(const Qubit_State* state, uint8_t qubit);

    // Backend info
    const char* (*name)(void);
    bool (*is_quantum)(void);
} Qubit_Backend_Ops;

// ============================================================================
// Backend Registry
// ============================================================================

// Get backend operations for a given type
const Qubit_Backend_Ops* get_backend_ops(Qubit_Backend_Type backend);

// List available backends
const char** list_available_backends(uint32_t* count);

// ============================================================================
// Classical Backend (Default)
// ============================================================================

typedef struct {
    uint8_t* bits;              // Classical bits (0 or 1)
} Classical_Qubit_State;

// Classical backend operations
extern const Qubit_Backend_Ops classical_backend_ops;

// ============================================================================
// Quantum Simulator Backend (Optional - compile with -DENABLE_QUANTUM_SIMULATOR)
// ============================================================================

#ifdef ENABLE_QUANTUM_SIMULATOR

typedef struct {
    // State vector: 2^n complex amplitudes
    // For n qubits: |ψ⟩ = Σ αᵢ|i⟩ where i ∈ {0,1}^n
    double* real_amplitudes;    // Real parts
    double* imag_amplitudes;    // Imaginary parts
    uint64_t state_size;        // 2^n
} Quantum_Simulator_State;

extern const Qubit_Backend_Ops quantum_simulator_ops;

#endif

// ============================================================================
// Quantum Hardware Backend (Optional - compile with -DENABLE_QUANTUM_HARDWARE)
// ============================================================================

#ifdef ENABLE_QUANTUM_HARDWARE

typedef struct {
    // Interface to quantum hardware (IBM Qiskit, Google Cirq, etc.)
    void* quantum_handle;
    char* device_name;
} Quantum_Hardware_State;

extern const Qubit_Backend_Ops quantum_hardware_ops;

#endif

// ============================================================================
// Convenience Functions
// ============================================================================

// Initialize qubit state with specified backend
Qubit_State* qubit_init(uint32_t n_qubits, Qubit_Backend_Type backend);

// Free qubit state
void qubit_free(Qubit_State* state);

// Clone qubit state (deep copy)
Qubit_State* qubit_clone(const Qubit_State* state);

// Apply gates (backend-agnostic)
void qubit_CCNOT(Qubit_State* state, uint8_t a, uint8_t b, uint8_t c);
void qubit_CNOT(Qubit_State* state, uint8_t a, uint8_t b);
void qubit_NOT(Qubit_State* state, uint8_t a);
void qubit_SWAP(Qubit_State* state, uint8_t a, uint8_t b);

// Measurement
uint8_t qubit_measure(Qubit_State* state, uint8_t qubit);
uint8_t qubit_read(const Qubit_State* state, uint8_t qubit);

// Backend info
const char* qubit_backend_name(const Qubit_State* state);
bool qubit_is_quantum(const Qubit_State* state);

#endif // MOOP_QUANTUM_READY_H
