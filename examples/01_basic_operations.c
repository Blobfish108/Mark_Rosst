// 01_basic_operations.c
// Demonstrates basic reversible gate operations

#include "../src/moop_enhanced.h"
#include <stdio.h>

int main() {
    printf("=== Moop Example 1: Basic Operations ===\n\n");

    // Initialize runtime with 8 qubits
    Moop_Runtime* moop = moop_init(8, 1);
    if (!moop) {
        fprintf(stderr, "Failed to initialize Moop runtime\n");
        return 1;
    }

    // Execute reversible operations
    printf("Executing reversible operations...\n");
    l2a_NOT(moop->l2a, 0);
    l2a_NOT(moop->l2a, 1);
    l2a_CNOT(moop->l2a, 0, 2);
    l2a_CCNOT(moop->l2a, 0, 1, 3);
    l2a_SWAP(moop->l2a, 2, 3);

    printf("Operations executed: %u\n", moop->l2a->total_ops);
    printf("Tape wrapped: %s\n\n", moop->l2a->tape_wrapped ? "Yes" : "No");

    // Print qubit states
    printf("Qubit states:\n");
    for (uint32_t i = 0; i < 4; i++) {
        printf("  q[%u] = %u\n", i, moop->l2a->qubits[i]);
    }

    // Cleanup
    moop_free(moop);
    printf("\n✓ Example complete\n");
    return 0;
}
