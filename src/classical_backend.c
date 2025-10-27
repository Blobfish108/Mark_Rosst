// classical_backend.c
// Classical qubit backend using conventional hardware (uint8_t bits)
// Default backend for Moop - fast, deterministic, works everywhere

#define _POSIX_C_SOURCE 200809L
#include "moop_quantum_ready.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ============================================================================
// Classical Backend Implementation
// ============================================================================

static Qubit_State* classical_init(uint32_t n_qubits) {
    Qubit_State* state = malloc(sizeof(Qubit_State));
    if (!state) return NULL;

    state->backend_type = QUBIT_BACKEND_CLASSICAL;
    state->qubit_count = n_qubits;
    state->metadata = NULL;

    Classical_Qubit_State* classical = malloc(sizeof(Classical_Qubit_State));
    if (!classical) {
        free(state);
        return NULL;
    }

    classical->bits = calloc(n_qubits, sizeof(uint8_t));
    if (!classical->bits) {
        free(classical);
        free(state);
        return NULL;
    }

    state->backend_data = classical;
    return state;
}

static void classical_free(Qubit_State* state) {
    if (!state) return;

    Classical_Qubit_State* classical =
        (Classical_Qubit_State*)state->backend_data;

    if (classical) {
        free(classical->bits);
        free(classical);
    }

    free(state);
}

static Qubit_State* classical_clone(const Qubit_State* state) {
    if (!state) return NULL;

    Qubit_State* cloned = classical_init(state->qubit_count);
    if (!cloned) return NULL;

    Classical_Qubit_State* src =
        (Classical_Qubit_State*)state->backend_data;
    Classical_Qubit_State* dst =
        (Classical_Qubit_State*)cloned->backend_data;

    memcpy(dst->bits, src->bits, state->qubit_count * sizeof(uint8_t));

    return cloned;
}

// ============================================================================
// Classical Reversible Gates
// ============================================================================

static void classical_CCNOT(Qubit_State* state, uint8_t a, uint8_t b, uint8_t c) {
    Classical_Qubit_State* classical =
        (Classical_Qubit_State*)state->backend_data;

    // Toffoli gate: if (a AND b) then flip c
    if (classical->bits[a] && classical->bits[b]) {
        classical->bits[c] ^= 1;
    }
}

static void classical_CNOT(Qubit_State* state, uint8_t a, uint8_t b) {
    Classical_Qubit_State* classical =
        (Classical_Qubit_State*)state->backend_data;

    // Controlled-NOT: if a then flip b
    if (classical->bits[a]) {
        classical->bits[b] ^= 1;
    }
}

static void classical_NOT(Qubit_State* state, uint8_t a) {
    Classical_Qubit_State* classical =
        (Classical_Qubit_State*)state->backend_data;

    // NOT gate: flip bit
    classical->bits[a] ^= 1;
}

static void classical_SWAP(Qubit_State* state, uint8_t a, uint8_t b) {
    Classical_Qubit_State* classical =
        (Classical_Qubit_State*)state->backend_data;

    // SWAP: exchange bits
    uint8_t temp = classical->bits[a];
    classical->bits[a] = classical->bits[b];
    classical->bits[b] = temp;
}

// ============================================================================
// Measurement (Trivial for Classical)
// ============================================================================

static uint8_t classical_measure(Qubit_State* state, uint8_t qubit) {
    // For classical: measurement is just reading (no collapse)
    Classical_Qubit_State* classical =
        (Classical_Qubit_State*)state->backend_data;

    return classical->bits[qubit];
}

static uint8_t classical_read(const Qubit_State* state, uint8_t qubit) {
    // Same as measure for classical backend
    const Classical_Qubit_State* classical =
        (const Classical_Qubit_State*)state->backend_data;

    return classical->bits[qubit];
}

// ============================================================================
// Backend Info
// ============================================================================

static const char* classical_name(void) {
    return "Classical (Conventional Hardware)";
}

static bool classical_is_quantum(void) {
    return false;
}

// ============================================================================
// Operations Table
// ============================================================================

const Qubit_Backend_Ops classical_backend_ops = {
    .init = classical_init,
    .free = classical_free,
    .clone = classical_clone,
    .CCNOT = classical_CCNOT,
    .CNOT = classical_CNOT,
    .NOT = classical_NOT,
    .SWAP = classical_SWAP,
    .measure = classical_measure,
    .read = classical_read,
    .name = classical_name,
    .is_quantum = classical_is_quantum
};
