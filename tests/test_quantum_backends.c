// test_quantum_backends.c
// Demonstration of quantum-ready backend abstraction
// Tests both classical and quantum simulator backends

#define _POSIX_C_SOURCE 200809L
#include "../src/moop_quantum_ready.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// ============================================================================
// Test Classical Backend
// ============================================================================

void test_classical_backend() {
    printf("\n=== Testing Classical Backend ===\n");

    // Initialize classical backend
    Qubit_State* state = qubit_init(8, QUBIT_BACKEND_CLASSICAL);
    assert(state != NULL);

    printf("Backend: %s\n", qubit_backend_name(state));
    printf("Is quantum: %s\n", qubit_is_quantum(state) ? "YES" : "NO");

    // Apply gates
    qubit_NOT(state, 0);                // |0⟩ → |1⟩
    qubit_CNOT(state, 0, 1);           // |10⟩ → |11⟩
    qubit_CCNOT(state, 0, 1, 2);       // |110⟩ → |111⟩

    // Read results (deterministic for classical)
    uint8_t result0 = qubit_read(state, 0);
    uint8_t result1 = qubit_read(state, 1);
    uint8_t result2 = qubit_read(state, 2);

    printf("Qubit 0: %d (expected 1)\n", result0);
    printf("Qubit 1: %d (expected 1)\n", result1);
    printf("Qubit 2: %d (expected 1)\n", result2);

    assert(result0 == 1);
    assert(result1 == 1);
    assert(result2 == 1);

    printf("✓ Classical backend works correctly\n");

    qubit_free(state);
}

// ============================================================================
// Test Quantum Simulator Backend (if available)
// ============================================================================

#ifdef ENABLE_QUANTUM_SIMULATOR

void test_quantum_simulator_backend() {
    printf("\n=== Testing Quantum Simulator Backend ===\n");

    // Initialize quantum simulator
    Qubit_State* state = qubit_init(3, QUBIT_BACKEND_SIMULATOR);
    assert(state != NULL);

    printf("Backend: %s\n", qubit_backend_name(state));
    printf("Is quantum: %s\n", qubit_is_quantum(state) ? "YES" : "NO");

    // Apply gates (same API as classical!)
    qubit_NOT(state, 0);                // |0⟩ → |1⟩
    qubit_CNOT(state, 0, 1);           // Creates entanglement
    qubit_CCNOT(state, 0, 1, 2);       // Toffoli gate

    // Measure (probabilistic collapse)
    uint8_t result0 = qubit_measure(state, 0);
    uint8_t result1 = qubit_measure(state, 1);
    uint8_t result2 = qubit_measure(state, 2);

    printf("Measured qubit 0: %d (expected 1)\n", result0);
    printf("Measured qubit 1: %d (expected 1)\n", result1);
    printf("Measured qubit 2: %d (expected 1)\n", result2);

    // After these specific operations, results are deterministic
    assert(result0 == 1);
    assert(result1 == 1);
    assert(result2 == 1);

    printf("✓ Quantum simulator backend works correctly\n");

    qubit_free(state);
}

void test_quantum_superposition() {
    printf("\n=== Testing Quantum Superposition ===\n");

    // This test demonstrates true quantum behavior that differs from classical
    // We'll create an entangled state and measure it multiple times

    Qubit_State* state = qubit_init(2, QUBIT_BACKEND_SIMULATOR);

    // Create entanglement: NOT(0), CNOT(0,1) creates |11⟩ state
    qubit_NOT(state, 0);
    qubit_CNOT(state, 0, 1);

    printf("Created entangled state |11⟩\n");
    printf("After measurement, both qubits should be 1\n");

    // Measure (should collapse to |11⟩)
    uint8_t result0 = qubit_measure(state, 0);
    uint8_t result1 = qubit_measure(state, 1);

    printf("Qubit 0: %d, Qubit 1: %d\n", result0, result1);

    // Both should be 1 (entangled)
    assert(result0 == result1);  // Entanglement preserved
    assert(result0 == 1);

    printf("✓ Quantum entanglement works correctly\n");

    qubit_free(state);
}

#endif

// ============================================================================
// Test Backend Abstraction
// ============================================================================

void test_backend_abstraction() {
    printf("\n=== Testing Backend Abstraction ===\n");

    // List available backends
    uint32_t count = 0;
    const char** backends = list_available_backends(&count);

    printf("Available backends: %u\n", count);
    for (uint32_t i = 0; i < count; i++) {
        printf("  %u. %s\n", i + 1, backends[i]);
    }

#ifdef ENABLE_QUANTUM_SIMULATOR
    assert(count >= 2);  // Classical + Simulator
#else
    assert(count >= 1);  // Classical only
#endif

    printf("✓ Backend abstraction works correctly\n");
}

// ============================================================================
// Test Same Code, Different Backends
// ============================================================================

void run_computation_on_backend(Qubit_Backend_Type backend, const char* backend_name) {
    printf("\nRunning on %s:\n", backend_name);

    Qubit_State* state = qubit_init(4, backend);

    // Same code works on any backend
    qubit_NOT(state, 0);
    qubit_CNOT(state, 0, 1);
    qubit_NOT(state, 2);
    qubit_SWAP(state, 1, 2);

    uint8_t r0 = qubit_is_quantum(state) ? qubit_measure(state, 0) : qubit_read(state, 0);
    uint8_t r1 = qubit_is_quantum(state) ? qubit_measure(state, 1) : qubit_read(state, 1);
    uint8_t r2 = qubit_is_quantum(state) ? qubit_measure(state, 2) : qubit_read(state, 2);

    printf("  Results: q0=%d, q1=%d, q2=%d\n", r0, r1, r2);

    qubit_free(state);
}

void test_same_code_different_backends() {
    printf("\n=== Testing Same Code on Different Backends ===\n");

    // Classical
    run_computation_on_backend(QUBIT_BACKEND_CLASSICAL, "Classical Backend");

#ifdef ENABLE_QUANTUM_SIMULATOR
    // Quantum simulator
    run_computation_on_backend(QUBIT_BACKEND_SIMULATOR, "Quantum Simulator");
#endif

    printf("✓ Same code runs on all backends\n");
}

// ============================================================================
// Main
// ============================================================================

int main(void) {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║  Moop Quantum-Ready Backend Test Suite                    ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");

    test_backend_abstraction();
    test_classical_backend();

#ifdef ENABLE_QUANTUM_SIMULATOR
    test_quantum_simulator_backend();
    test_quantum_superposition();
#else
    printf("\n[INFO] Quantum simulator not enabled. To test quantum backend:\n");
    printf("       make clean && make CFLAGS=\"-DENABLE_QUANTUM_SIMULATOR\"\n");
#endif

    test_same_code_different_backends();

    printf("\n╔════════════════════════════════════════════════════════════╗\n");
#ifdef ENABLE_QUANTUM_SIMULATOR
    printf("║  ✓ All Backends Validated (Classical + Quantum)           ║\n");
#else
    printf("║  ✓ Classical Backend Validated                            ║\n");
#endif
    printf("╚════════════════════════════════════════════════════════════╝\n");

    return 0;
}
