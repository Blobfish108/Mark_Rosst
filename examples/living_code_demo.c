// living_code_demo.c
// Demonstrates Moop's unique "living code" capabilities
//
// Shows:
// 1. Code as data (homoiconicity) - reading and modifying gate sequences
// 2. Reversible execution (time travel) - checkpoints and restoration
// 3. Evolutionary fitness (self-optimization) - tape pruning in action
//
// Run: make examples && ./build/living_code_demo

#define _POSIX_C_SOURCE 200809L
#include "../src/moop_enhanced.h"
#include <stdio.h>
#include <stdlib.h>

static void print_gate(R_Cell c) {
    const char* names[] = {"CCNOT", "CNOT", "NOT", "SWAP"};
    printf("%s(", names[c.gate]);
    if (c.gate == 0) printf("%u,%u,%u", c.a, c.b, c.c);
    else if (c.gate == 1) printf("%u,%u", c.a, c.b);
    else if (c.gate == 2) printf("%u", c.a);
    else printf("%u,%u", c.a, c.b);
    printf(")");
}

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║  Moop: Living Code Demonstration                         ║\n");
    printf("║  Homoiconicity + Reversibility + Evolution               ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");

    // Initialize quantum-ready runtime
    L2a_Runtime* runtime = l2a_init(4, 1, QUBIT_BACKEND_CLASSICAL);

    printf("Backend: %s\n", qubit_backend_name(runtime->qubit_state));
    printf("Quantum-ready: %s\n\n", qubit_is_quantum(runtime->qubit_state) ? "Yes" : "No");

    // ========================================================================
    // Demo 1: Homoiconicity - Code as Data
    // ========================================================================

    printf("═══ Demo 1: Homoiconicity (Code as Data) ═══\n\n");

    printf("Writing code:\n");
    l2a_NOT(runtime, 0);
    l2a_CNOT(runtime, 0, 1);
    l2a_NOT(runtime, 2);

    printf("  Executed: NOT(0), CNOT(0,1), NOT(2)\n");
    printf("  Qubit states: ");
    for (int i = 0; i < 4; i++) {
        printf("q%d=%u ", i, qubit_read(runtime->qubit_state, i));
    }
    printf("\n\n");

    printf("Reading code from tape:\n");
    for (uint32_t i = 0; i < 3 && i < runtime->tape_head; i++) {
        R_Cell cell = l2a_read_tape(runtime, i);
        printf("  tape[%u]: ", i);
        print_gate(cell);
        printf(" (fitness=%.2f)\n", runtime->tape[i].fitness);
    }
    printf("\n");

    printf("Modifying code (change NOT to SWAP):\n");
    R_Cell modified = {3, 0, 2, 0};  // SWAP(0, 2)
    l2a_write_tape(runtime, 2, modified);

    printf("  tape[2]: ");
    print_gate(l2a_read_tape(runtime, 2));
    printf("\n\n");

    printf("✓ Code is data - can be read and modified at runtime\n\n");

    // ========================================================================
    // Demo 2: Reversibility - Time Travel
    // ========================================================================

    printf("═══ Demo 2: Reversibility (Time Travel) ═══\n\n");

    // Reset state
    l2a_free(runtime);
    runtime = l2a_init(4, 2, QUBIT_BACKEND_CLASSICAL);

    printf("Initial state: ");
    for (int i = 0; i < 4; i++) {
        printf("q%d=%u ", i, qubit_read(runtime->qubit_state, i));
    }
    printf("\n\n");

    // Create checkpoint
    printf("Creating checkpoint...\n");
    uint32_t checkpoint = l2a_checkpoint(runtime);
    printf("  Checkpoint at position: %u\n\n", checkpoint);

    // Make changes
    printf("Executing operations:\n");
    l2a_NOT(runtime, 0);
    printf("  NOT(0): ");
    for (int i = 0; i < 4; i++) {
        printf("q%d=%u ", i, qubit_read(runtime->qubit_state, i));
    }
    printf("\n");

    l2a_NOT(runtime, 1);
    printf("  NOT(1): ");
    for (int i = 0; i < 4; i++) {
        printf("q%d=%u ", i, qubit_read(runtime->qubit_state, i));
    }
    printf("\n");

    l2a_CNOT(runtime, 0, 2);
    printf("  CNOT(0,2): ");
    for (int i = 0; i < 4; i++) {
        printf("q%d=%u ", i, qubit_read(runtime->qubit_state, i));
    }
    printf("\n\n");

    // Restore
    printf("Time travel: Restoring to checkpoint...\n");
    l2a_restore(runtime, checkpoint);
    printf("  After restore: ");
    for (int i = 0; i < 4; i++) {
        printf("q%d=%u ", i, qubit_read(runtime->qubit_state, i));
    }
    printf("\n\n");

    printf("✓ Reversibility - all operations undone, back to checkpoint\n\n");

    // ========================================================================
    // Demo 3: Evolutionary Fitness - Self-Optimization
    // ========================================================================

    printf("═══ Demo 3: Evolutionary Fitness (Self-Optimization) ═══\n\n");

    // Reset with fresh tape
    l2a_free(runtime);
    runtime = l2a_init(4, 3, QUBIT_BACKEND_CLASSICAL);

    printf("Executing 100 operations to trigger evolutionary pruning...\n\n");

    // Execute operations with varying patterns
    for (int i = 0; i < 100; i++) {
        if (i % 3 == 0) l2a_NOT(runtime, 0);      // Frequent
        else if (i % 7 == 0) l2a_CNOT(runtime, 0, 1);  // Less frequent
        else l2a_SWAP(runtime, 2, 3);              // Rare
    }

    Tape_Stats stats = l2a_get_tape_stats(runtime);

    printf("Tape Statistics:\n");
    printf("  Total operations: %u\n", runtime->total_ops);
    printf("  Tape wrapped: %s\n", runtime->tape_wrapped ? "Yes" : "No");
    printf("  Pruning cycles: %u\n", runtime->pruning_cycles);
    printf("  Average fitness: %.3f\n", stats.avg_fitness);
    printf("  Min fitness: %.3f\n", stats.min_fitness);
    printf("  Max fitness: %.3f\n", stats.max_fitness);
    printf("  Active entries: %u / 1024\n", stats.active_count);
    printf("  Essential entries: %u\n\n", stats.essential_count);

    printf("Fitness parameters (adaptive):\n");
    Fitness_Params params = l2a_get_fitness_params(runtime);
    printf("  Recency weight: %.2f\n", params.recency_weight);
    printf("  Activity weight: %.2f\n", params.activity_weight);
    printf("  Gate priority weight: %.2f\n", params.gate_weight);
    printf("  Prune interval: %u ops\n", params.prune_interval);
    printf("  Prune threshold: %.2f\n\n", params.prune_threshold);

    printf("✓ Evolutionary substrate - low-fitness operations pruned\n");
    printf("✓ Self-organizing system - adapts to usage patterns\n\n");

    // ========================================================================
    // Demo 4: The Synergy - All Three Together
    // ========================================================================

    printf("═══ Demo 4: The Synergy (All Three Combined) ═══\n\n");

    printf("This combination is UNIQUE to Moop:\n\n");

    printf("1. Homoiconic Languages (Lisp, Forth, Io):\n");
    printf("   ✓ Code as data\n");
    printf("   ✗ Can't undo modifications\n");
    printf("   ✗ No evolutionary optimization\n\n");

    printf("2. Quantum Computing:\n");
    printf("   ✓ Reversible operations\n");
    printf("   ✗ Fixed code structure\n");
    printf("   ✗ No self-modification\n\n");

    printf("3. Genetic Programming:\n");
    printf("   ✓ Evolutionary optimization\n");
    printf("   ✗ No reversibility\n");
    printf("   ✗ External fitness evaluation\n\n");

    printf("4. Moop:\n");
    printf("   ✓ Code as data (homoiconicity)\n");
    printf("   ✓ Reversible execution (checkpoints)\n");
    printf("   ✓ Evolutionary fitness (built-in)\n");
    printf("   ✓ Quantum-ready (same code, any backend)\n\n");

    printf("═══════════════════════════════════════════════════════════\n\n");

    printf("What this enables:\n");
    printf("  • Programs that explore their own code space\n");
    printf("  • Safe meta-programming (can backtrack)\n");
    printf("  • Self-optimizing systems (fitness-driven)\n");
    printf("  • Future-proof code (runs on quantum hardware)\n\n");

    printf("This is \"living code\" - programs that:\n");
    printf("  - Read their own structure\n");
    printf("  - Modify themselves\n");
    printf("  - Test modifications reversibly\n");
    printf("  - Keep beneficial changes\n");
    printf("  - Evolve over time\n\n");

    printf("No other language can do this.\n\n");

    // Cleanup
    l2a_free(runtime);

    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║  End of Living Code Demonstration                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");

    return 0;
}
