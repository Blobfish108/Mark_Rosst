// quantum_simulator_backend.c
// Quantum simulator backend using statevector representation
// Optional backend (compile with -DENABLE_QUANTUM_SIMULATOR)
// Demonstrates true quantum superposition and entanglement

#define _POSIX_C_SOURCE 200809L
#include "moop_quantum_ready.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#ifdef ENABLE_QUANTUM_SIMULATOR

// ============================================================================
// Quantum Simulator State Representation
// ============================================================================
// For n qubits: |ψ⟩ = Σᵢ αᵢ|i⟩ where i ∈ {0,1}ⁿ
// State vector has 2ⁿ complex amplitudes
// (Quantum_Simulator_State is defined in moop_quantum_ready.h)
// ============================================================================

// ============================================================================
// Helper Functions
// ============================================================================

static inline uint64_t pow2(uint32_t n) {
    return 1ULL << n;
}

static void normalize_statevector(Quantum_Simulator_State* qstate) {
    // Calculate norm: Σᵢ |αᵢ|²
    double norm_sq = 0.0;
    for (uint64_t i = 0; i < qstate->state_size; i++) {
        double r = qstate->real_amplitudes[i];
        double im = qstate->imag_amplitudes[i];
        norm_sq += r*r + im*im;
    }

    if (norm_sq < 1e-10) {
        // Degenerate state - reinitialize to |0⟩
        qstate->real_amplitudes[0] = 1.0;
        qstate->imag_amplitudes[0] = 0.0;
        for (uint64_t i = 1; i < qstate->state_size; i++) {
            qstate->real_amplitudes[i] = 0.0;
            qstate->imag_amplitudes[i] = 0.0;
        }
        return;
    }

    double norm = sqrt(norm_sq);
    for (uint64_t i = 0; i < qstate->state_size; i++) {
        qstate->real_amplitudes[i] /= norm;
        qstate->imag_amplitudes[i] /= norm;
    }
}

// ============================================================================
// Lifecycle Functions
// ============================================================================

static Qubit_State* quantum_simulator_init(uint32_t n_qubits) {
    if (n_qubits > 20) {
        // Statevector simulation becomes impractical beyond ~20 qubits
        fprintf(stderr, "Warning: %u qubits requires 2^%u = %llu amplitudes\n",
                n_qubits, n_qubits, pow2(n_qubits));
        fprintf(stderr, "Memory required: ~%.2f GB\n",
                (pow2(n_qubits) * 2 * sizeof(double)) / (1024.0*1024.0*1024.0));
    }

    Qubit_State* state = malloc(sizeof(Qubit_State));
    if (!state) return NULL;

    state->backend_type = QUBIT_BACKEND_SIMULATOR;
    state->qubit_count = n_qubits;
    state->metadata = NULL;

    Quantum_Simulator_State* qstate = malloc(sizeof(Quantum_Simulator_State));
    if (!qstate) {
        free(state);
        return NULL;
    }

    qstate->state_size = pow2(n_qubits);
    qstate->real_amplitudes = calloc(qstate->state_size, sizeof(double));
    qstate->imag_amplitudes = calloc(qstate->state_size, sizeof(double));

    if (!qstate->real_amplitudes || !qstate->imag_amplitudes) {
        free(qstate->real_amplitudes);
        free(qstate->imag_amplitudes);
        free(qstate);
        free(state);
        return NULL;
    }

    // Initialize to |0...0⟩ state
    qstate->real_amplitudes[0] = 1.0;
    qstate->imag_amplitudes[0] = 0.0;

    state->backend_data = qstate;
    return state;
}

static void quantum_simulator_free(Qubit_State* state) {
    if (!state) return;

    Quantum_Simulator_State* qstate =
        (Quantum_Simulator_State*)state->backend_data;

    if (qstate) {
        free(qstate->real_amplitudes);
        free(qstate->imag_amplitudes);
        free(qstate);
    }

    free(state);
}

static Qubit_State* quantum_simulator_clone(const Qubit_State* state) {
    if (!state) return NULL;

    Qubit_State* cloned = quantum_simulator_init(state->qubit_count);
    if (!cloned) return NULL;

    Quantum_Simulator_State* src =
        (Quantum_Simulator_State*)state->backend_data;
    Quantum_Simulator_State* dst =
        (Quantum_Simulator_State*)cloned->backend_data;

    memcpy(dst->real_amplitudes, src->real_amplitudes,
           src->state_size * sizeof(double));
    memcpy(dst->imag_amplitudes, src->imag_amplitudes,
           src->state_size * sizeof(double));

    return cloned;
}

// ============================================================================
// Quantum Gate Implementations
// ============================================================================
// Apply unitary matrices to statevector
// For gate G on qubits q₀, q₁, ...:
//   |ψ'⟩ = G|ψ⟩
// ============================================================================

static void quantum_simulator_NOT(Qubit_State* state, uint8_t target) {
    Quantum_Simulator_State* qstate =
        (Quantum_Simulator_State*)state->backend_data;

    uint64_t target_mask = pow2(target);

    // NOT gate: swap amplitudes for basis states differing in target qubit
    for (uint64_t i = 0; i < qstate->state_size; i++) {
        if (i & target_mask) continue; // Already processed

        uint64_t j = i | target_mask; // Flip target bit

        // Swap amplitudes
        double temp_r = qstate->real_amplitudes[i];
        double temp_im = qstate->imag_amplitudes[i];

        qstate->real_amplitudes[i] = qstate->real_amplitudes[j];
        qstate->imag_amplitudes[i] = qstate->imag_amplitudes[j];

        qstate->real_amplitudes[j] = temp_r;
        qstate->imag_amplitudes[j] = temp_im;
    }
}

static void quantum_simulator_CNOT(Qubit_State* state, uint8_t control, uint8_t target) {
    Quantum_Simulator_State* qstate =
        (Quantum_Simulator_State*)state->backend_data;

    uint64_t control_mask = pow2(control);
    uint64_t target_mask = pow2(target);

    // CNOT: flip target if control is 1
    for (uint64_t i = 0; i < qstate->state_size; i++) {
        // Only act when control bit is 1 and we haven't processed this pair
        if (!(i & control_mask)) continue;
        if (i & target_mask) continue;

        uint64_t j = i | target_mask; // Flip target bit

        // Swap amplitudes
        double temp_r = qstate->real_amplitudes[i];
        double temp_im = qstate->imag_amplitudes[i];

        qstate->real_amplitudes[i] = qstate->real_amplitudes[j];
        qstate->imag_amplitudes[i] = qstate->imag_amplitudes[j];

        qstate->real_amplitudes[j] = temp_r;
        qstate->imag_amplitudes[j] = temp_im;
    }
}

static void quantum_simulator_CCNOT(Qubit_State* state, uint8_t ctrl1, uint8_t ctrl2, uint8_t target) {
    Quantum_Simulator_State* qstate =
        (Quantum_Simulator_State*)state->backend_data;

    uint64_t ctrl1_mask = pow2(ctrl1);
    uint64_t ctrl2_mask = pow2(ctrl2);
    uint64_t target_mask = pow2(target);

    // CCNOT (Toffoli): flip target if both controls are 1
    for (uint64_t i = 0; i < qstate->state_size; i++) {
        // Only act when both control bits are 1
        if (!(i & ctrl1_mask)) continue;
        if (!(i & ctrl2_mask)) continue;
        if (i & target_mask) continue;

        uint64_t j = i | target_mask; // Flip target bit

        // Swap amplitudes
        double temp_r = qstate->real_amplitudes[i];
        double temp_im = qstate->imag_amplitudes[i];

        qstate->real_amplitudes[i] = qstate->real_amplitudes[j];
        qstate->imag_amplitudes[i] = qstate->imag_amplitudes[j];

        qstate->real_amplitudes[j] = temp_r;
        qstate->imag_amplitudes[j] = temp_im;
    }
}

static void quantum_simulator_SWAP(Qubit_State* state, uint8_t qubit1, uint8_t qubit2) {
    Quantum_Simulator_State* qstate =
        (Quantum_Simulator_State*)state->backend_data;

    uint64_t mask1 = pow2(qubit1);
    uint64_t mask2 = pow2(qubit2);

    // SWAP: exchange qubits (swap amplitudes for states differing in these bits)
    for (uint64_t i = 0; i < qstate->state_size; i++) {
        uint8_t bit1 = (i & mask1) ? 1 : 0;
        uint8_t bit2 = (i & mask2) ? 1 : 0;

        if (bit1 == bit2) continue; // Same state after swap

        // Compute swapped index
        uint64_t j = i;
        if (bit1) {
            j &= ~mask1; // Clear bit1
            j |= mask2;  // Set bit2
        } else {
            j |= mask1;  // Set bit1
            j &= ~mask2; // Clear bit2
        }

        if (i >= j) continue; // Only process each pair once

        // Swap amplitudes
        double temp_r = qstate->real_amplitudes[i];
        double temp_im = qstate->imag_amplitudes[i];

        qstate->real_amplitudes[i] = qstate->real_amplitudes[j];
        qstate->imag_amplitudes[i] = qstate->imag_amplitudes[j];

        qstate->real_amplitudes[j] = temp_r;
        qstate->imag_amplitudes[j] = temp_im;
    }
}

// ============================================================================
// Measurement (Collapses Quantum State)
// ============================================================================

static uint8_t quantum_simulator_measure(Qubit_State* state, uint8_t qubit) {
    Quantum_Simulator_State* qstate =
        (Quantum_Simulator_State*)state->backend_data;

    uint64_t qubit_mask = pow2(qubit);

    // Calculate probability of measuring |0⟩ on target qubit
    double prob_zero = 0.0;
    for (uint64_t i = 0; i < qstate->state_size; i++) {
        if (i & qubit_mask) continue; // Skip |1⟩ states

        double r = qstate->real_amplitudes[i];
        double im = qstate->imag_amplitudes[i];
        prob_zero += r*r + im*im;
    }

    // Random measurement outcome based on Born rule
    double random = (double)rand() / RAND_MAX;
    uint8_t outcome = (random < prob_zero) ? 0 : 1;

    // Collapse state: zero out amplitudes inconsistent with measurement
    for (uint64_t i = 0; i < qstate->state_size; i++) {
        uint8_t bit = (i & qubit_mask) ? 1 : 0;
        if (bit != outcome) {
            qstate->real_amplitudes[i] = 0.0;
            qstate->imag_amplitudes[i] = 0.0;
        }
    }

    // Renormalize
    normalize_statevector(qstate);

    return outcome;
}

static uint8_t quantum_simulator_read(const Qubit_State* state, uint8_t qubit) {
    // For quantum simulator: read performs measurement (cannot read without collapse)
    // Note: This violates const, but that's the nature of quantum measurement
    return quantum_simulator_measure((Qubit_State*)state, qubit);
}

// ============================================================================
// Backend Info
// ============================================================================

static const char* quantum_simulator_name(void) {
    return "Quantum Simulator (Statevector)";
}

static bool quantum_simulator_is_quantum(void) {
    return true;
}

// ============================================================================
// Operations Table
// ============================================================================

const Qubit_Backend_Ops quantum_simulator_ops = {
    .init = quantum_simulator_init,
    .free = quantum_simulator_free,
    .clone = quantum_simulator_clone,
    .CCNOT = quantum_simulator_CCNOT,
    .CNOT = quantum_simulator_CNOT,
    .NOT = quantum_simulator_NOT,
    .SWAP = quantum_simulator_SWAP,
    .measure = quantum_simulator_measure,
    .read = quantum_simulator_read,
    .name = quantum_simulator_name,
    .is_quantum = quantum_simulator_is_quantum
};

#endif // ENABLE_QUANTUM_SIMULATOR
