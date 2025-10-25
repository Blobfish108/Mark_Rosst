// 02_evolutionary_pruning.c
// Demonstrates evolutionary pruning and fitness tracking

#include "../src/moop_enhanced.h"
#include <stdio.h>

int main() {
    printf("=== Moop Example 2: Evolutionary Pruning ===\n\n");

    Moop_Runtime* moop = moop_init(16, 2);
    if (!moop) return 1;

    // Execute many operations to trigger pruning
    printf("Executing 1500 operations to trigger pruning...\n");
    for (uint32_t i = 0; i < 1500; i++) {
        l2a_NOT(moop->l2a, i % 16);
        if (i % 256 == 0 && i > 0) {
            printf("  Operation %u: Pruning cycle triggered\n", i);
        }
    }

    // Get tape statistics
    Tape_Stats stats = l2a_get_tape_stats(moop->l2a);

    printf("\nTape Statistics:\n");
    printf("  Average fitness: %.3f\n", stats.avg_fitness);
    printf("  Min fitness: %.3f\n", stats.min_fitness);
    printf("  Max fitness: %.3f\n", stats.max_fitness);
    printf("  Active entries: %u / 1024\n", stats.active_count);
    printf("  Essential entries: %u\n", stats.essential_count);
    printf("  Pruning cycles: %u\n", stats.pruning_cycles);
    printf("  Total operations: %u\n", moop->l2a->total_ops);
    printf("  Tape wrapped: %s\n", moop->l2a->tape_wrapped ? "Yes" : "No");

    // Inspect individual fitness values
    printf("\nFitness of first 5 tape entries:\n");
    for (uint32_t i = 0; i < 5; i++) {
        Tape_Entry entry = l2a_get_tape_entry(moop->l2a, i);
        printf("  Entry[%u]: fitness=%.3f, gate=%u, essential=%s\n",
               i, entry.fitness, entry.cell.gate,
               entry.essential ? "yes" : "no");
    }

    moop_free(moop);
    printf("\n✓ Example complete\n");
    return 0;
}
