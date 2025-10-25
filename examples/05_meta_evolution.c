// 05_meta_evolution.c
// Demonstrates meta-evolution: tuning fitness parameters

#include "../src/moop_enhanced.h"
#include <stdio.h>

int main() {
    printf("=== Moop Example 5: Meta-Evolution ===\n\n");

    Moop_Runtime* moop = moop_init(16, 5);
    if (!moop) return 1;

    // Get initial fitness parameters
    printf("Initial fitness parameters:\n");
    Fitness_Params params = l2a_get_fitness_params(moop->l2a);
    printf("  Recency weight: %.2f\n", params.recency_weight);
    printf("  Activity weight: %.2f\n", params.activity_weight);
    printf("  Gate weight: %.2f\n", params.gate_weight);
    printf("  Prune interval: %u ops\n", params.prune_interval);
    printf("  Prune threshold: %.2f (keep top %.0f%%)\n",
           params.prune_threshold, params.prune_threshold * 100);

    // Execute operations with default parameters
    printf("\nExecuting 512 operations with default parameters...\n");
    for (uint32_t i = 0; i < 512; i++) {
        l2a_CNOT(moop->l2a, i % 16, (i + 1) % 16);
    }

    Tape_Stats stats1 = l2a_get_tape_stats(moop->l2a);
    printf("  Pruning cycles: %u\n", stats1.pruning_cycles);
    printf("  Average fitness: %.3f\n", stats1.avg_fitness);

    // Tune fitness parameters (meta-evolution)
    printf("\nTuning fitness parameters (meta-evolution):\n");
    printf("  - Increase recency weight (0.5 -> 0.7)\n");
    printf("  - Decrease gate weight (0.2 -> 0.1)\n");
    printf("  - Prune more aggressively (keep 70%% instead of 75%%)\n");

    Fitness_Params new_params = {
        .recency_weight = 0.7f,    // Favor recent operations more
        .activity_weight = 0.2f,   // Slightly less important
        .gate_weight = 0.1f,       // Less important
        .prune_interval = 256,     // Same interval
        .prune_threshold = 0.70f   // Keep top 70% (more aggressive)
    };

    l2a_tune_fitness(moop->l2a, new_params);

    // Verify tuning
    Fitness_Params tuned = l2a_get_fitness_params(moop->l2a);
    printf("\nTuned parameters:\n");
    printf("  Recency weight: %.2f (weights auto-normalized)\n", tuned.recency_weight);
    printf("  Activity weight: %.2f\n", tuned.activity_weight);
    printf("  Gate weight: %.2f\n", tuned.gate_weight);
    printf("  Prune threshold: %.2f (keep top %.0f%%)\n",
           tuned.prune_threshold, tuned.prune_threshold * 100);

    // Execute more operations with tuned parameters
    printf("\nExecuting 512 more operations with tuned parameters...\n");
    for (uint32_t i = 0; i < 512; i++) {
        l2a_NOT(moop->l2a, i % 16);
    }

    Tape_Stats stats2 = l2a_get_tape_stats(moop->l2a);
    printf("  Pruning cycles: %u (total)\n", stats2.pruning_cycles);
    printf("  Average fitness: %.3f\n", stats2.avg_fitness);

    printf("\nFitness comparison:\n");
    printf("  Before tuning: %.3f\n", stats1.avg_fitness);
    printf("  After tuning: %.3f\n", stats2.avg_fitness);
    printf("  Change: %+.3f\n", stats2.avg_fitness - stats1.avg_fitness);

    moop_free(moop);
    printf("\n✓ Example complete: System evolved its own evolution!\n");
    return 0;
}
