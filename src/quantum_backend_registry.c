// quantum_backend_registry.c
// Backend registry and convenience functions for quantum-ready abstraction
// Provides runtime backend selection and unified API

#define _POSIX_C_SOURCE 200809L
#include "moop_quantum_ready.h"
#include <stdlib.h>
#include <stdio.h>

// ============================================================================
// Backend Registry
// ============================================================================

const Qubit_Backend_Ops* get_backend_ops(Qubit_Backend_Type backend) {
    switch (backend) {
        case QUBIT_BACKEND_CLASSICAL:
            return &classical_backend_ops;

#ifdef ENABLE_QUANTUM_SIMULATOR
        case QUBIT_BACKEND_SIMULATOR:
            return &quantum_simulator_ops;
#endif

#ifdef ENABLE_QUANTUM_HARDWARE
        case QUBIT_BACKEND_QUANTUM:
            return &quantum_hardware_ops;
#endif

        default:
            // Safe fallback to classical
            fprintf(stderr, "Warning: Unknown backend type %d, using classical\n", backend);
            return &classical_backend_ops;
    }
}

const char** list_available_backends(uint32_t* count) {
    static const char* backends[3];
    uint32_t idx = 0;

    // Classical is always available
    backends[idx++] = "Classical (Conventional Hardware)";

#ifdef ENABLE_QUANTUM_SIMULATOR
    backends[idx++] = "Quantum Simulator (Statevector)";
#endif

#ifdef ENABLE_QUANTUM_HARDWARE
    backends[idx++] = "Quantum Hardware (Real QPU)";
#endif

    *count = idx;
    return backends;
}

// ============================================================================
// Convenience Functions - Lifecycle
// ============================================================================

Qubit_State* qubit_init(uint32_t n_qubits, Qubit_Backend_Type backend) {
    const Qubit_Backend_Ops* ops = get_backend_ops(backend);
    if (!ops || !ops->init) {
        fprintf(stderr, "Error: Backend init not available\n");
        return NULL;
    }
    return ops->init(n_qubits);
}

void qubit_free(Qubit_State* state) {
    if (!state) return;

    const Qubit_Backend_Ops* ops = get_backend_ops(state->backend_type);
    if (!ops || !ops->free) {
        fprintf(stderr, "Error: Backend free not available\n");
        return;
    }
    ops->free(state);
}

Qubit_State* qubit_clone(const Qubit_State* state) {
    if (!state) return NULL;

    const Qubit_Backend_Ops* ops = get_backend_ops(state->backend_type);
    if (!ops || !ops->clone) {
        fprintf(stderr, "Error: Backend clone not available\n");
        return NULL;
    }
    return ops->clone(state);
}

// ============================================================================
// Convenience Functions - Gates
// ============================================================================

void qubit_CCNOT(Qubit_State* state, uint8_t a, uint8_t b, uint8_t c) {
    if (!state) return;

    const Qubit_Backend_Ops* ops = get_backend_ops(state->backend_type);
    if (!ops || !ops->CCNOT) {
        fprintf(stderr, "Error: Backend CCNOT not available\n");
        return;
    }
    ops->CCNOT(state, a, b, c);
}

void qubit_CNOT(Qubit_State* state, uint8_t a, uint8_t b) {
    if (!state) return;

    const Qubit_Backend_Ops* ops = get_backend_ops(state->backend_type);
    if (!ops || !ops->CNOT) {
        fprintf(stderr, "Error: Backend CNOT not available\n");
        return;
    }
    ops->CNOT(state, a, b);
}

void qubit_NOT(Qubit_State* state, uint8_t a) {
    if (!state) return;

    const Qubit_Backend_Ops* ops = get_backend_ops(state->backend_type);
    if (!ops || !ops->NOT) {
        fprintf(stderr, "Error: Backend NOT not available\n");
        return;
    }
    ops->NOT(state, a);
}

void qubit_SWAP(Qubit_State* state, uint8_t a, uint8_t b) {
    if (!state) return;

    const Qubit_Backend_Ops* ops = get_backend_ops(state->backend_type);
    if (!ops || !ops->SWAP) {
        fprintf(stderr, "Error: Backend SWAP not available\n");
        return;
    }
    ops->SWAP(state, a, b);
}

// ============================================================================
// Convenience Functions - Measurement
// ============================================================================

uint8_t qubit_measure(Qubit_State* state, uint8_t qubit) {
    if (!state) return 0;

    const Qubit_Backend_Ops* ops = get_backend_ops(state->backend_type);
    if (!ops || !ops->measure) {
        fprintf(stderr, "Error: Backend measure not available\n");
        return 0;
    }
    return ops->measure(state, qubit);
}

uint8_t qubit_read(const Qubit_State* state, uint8_t qubit) {
    if (!state) return 0;

    const Qubit_Backend_Ops* ops = get_backend_ops(state->backend_type);
    if (!ops || !ops->read) {
        fprintf(stderr, "Error: Backend read not available\n");
        return 0;
    }
    return ops->read(state, qubit);
}

// ============================================================================
// Convenience Functions - Info
// ============================================================================

const char* qubit_backend_name(const Qubit_State* state) {
    if (!state) return "Unknown";

    const Qubit_Backend_Ops* ops = get_backend_ops(state->backend_type);
    if (!ops || !ops->name) {
        return "Unknown";
    }
    return ops->name();
}

bool qubit_is_quantum(const Qubit_State* state) {
    if (!state) return false;

    const Qubit_Backend_Ops* ops = get_backend_ops(state->backend_type);
    if (!ops || !ops->is_quantum) {
        return false;
    }
    return ops->is_quantum();
}
