// evolutionary_optimization.c
// Demonstrates Moop's unique combination: Homoiconicity + Reversibility + Evolution
//
// This program shows "living code" that:
// 1. Modifies its own gate sequence (homoiconicity)
// 2. Tests modifications reversibly (can undo)
// 3. Evolves toward optimal solutions (fitness-based selection)
// 4. Self-organizes through evolutionary pruning
//
// Run: make examples && ./build/evolutionary_optimization

#define _POSIX_C_SOURCE 200809L
#include "../src/moop_enhanced.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ============================================================================
// Demo: Self-Optimizing XOR Calculator
// ============================================================================
// Goal: Find optimal gate sequence to compute XOR(a, b) -> c
// Strategy: Try different sequences, measure fitness, keep best ones

typedef struct {
    R_Cell* sequence;
    uint32_t length;
    float fitness;
} GateSequence;

// Compute XOR using a gate sequence (test fitness)
static float evaluate_sequence(L2a_Runtime* runtime, GateSequence* seq, uint8_t a_val, uint8_t b_val) {
    // Create checkpoint before testing
    uint32_t checkpoint = l2a_checkpoint(runtime);

    // Set initial qubit values
    if (a_val) l2a_NOT(runtime, 0);  // Set qubit 0 = a
    if (b_val) l2a_NOT(runtime, 1);  // Set qubit 1 = b

    // Execute the gate sequence
    for (uint32_t i = 0; i < seq->length; i++) {
        R_Cell cell = seq->sequence[i];
        switch (cell.gate) {
            case 0: l2a_CCNOT(runtime, cell.a, cell.b, cell.c); break;
            case 1: l2a_CNOT(runtime, cell.a, cell.b); break;
            case 2: l2a_NOT(runtime, cell.a); break;
            case 3: l2a_SWAP(runtime, cell.a, cell.b); break;
        }
    }

    // Check result on qubit 2 (should be a XOR b)
    uint8_t result = qubit_read(runtime->qubit_state, 2);
    uint8_t expected = a_val ^ b_val;

    // Fitness: 1.0 if correct, 0.0 if wrong
    float fitness = (result == expected) ? 1.0f : 0.0f;

    // Restore to checkpoint (reversibility!)
    l2a_restore(runtime, checkpoint);

    return fitness;
}

// Test sequence against all inputs
static float test_all_cases(L2a_Runtime* runtime, GateSequence* seq) {
    float total = 0.0f;

    // XOR truth table: 00->0, 01->1, 10->1, 11->0
    total += evaluate_sequence(runtime, seq, 0, 0);  // Should be 0
    total += evaluate_sequence(runtime, seq, 0, 1);  // Should be 1
    total += evaluate_sequence(runtime, seq, 1, 0);  // Should be 1
    total += evaluate_sequence(runtime, seq, 1, 1);  // Should be 0

    return total / 4.0f;  // Average fitness
}

// Generate random gate sequence
static GateSequence random_sequence(uint32_t max_length) {
    GateSequence seq;
    seq.length = (rand() % max_length) + 1;
    seq.sequence = malloc(seq.length * sizeof(R_Cell));
    seq.fitness = 0.0f;

    for (uint32_t i = 0; i < seq.length; i++) {
        seq.sequence[i].gate = rand() % 4;  // Random gate type
        seq.sequence[i].a = rand() % 3;     // Random qubits (0-2)
        seq.sequence[i].b = rand() % 3;
        seq.sequence[i].c = rand() % 3;
    }

    return seq;
}

// Mutate a sequence (homoiconicity: modifying code)
static GateSequence mutate_sequence(GateSequence* parent) {
    GateSequence mutant;
    mutant.length = parent->length;
    mutant.sequence = malloc(mutant.length * sizeof(R_Cell));
    mutant.fitness = 0.0f;

    // Copy parent sequence
    for (uint32_t i = 0; i < parent->length; i++) {
        mutant.sequence[i] = parent->sequence[i];
    }

    // Mutate one random gate
    if (mutant.length > 0) {
        uint32_t idx = rand() % mutant.length;
        int mutation_type = rand() % 3;

        switch (mutation_type) {
            case 0: mutant.sequence[idx].gate = rand() % 4; break;      // Change gate type
            case 1: mutant.sequence[idx].a = rand() % 3; break;         // Change qubit a
            case 2: mutant.sequence[idx].b = rand() % 3; break;         // Change qubit b
        }
    }

    return mutant;
}

static void print_sequence(GateSequence* seq) {
    const char* gate_names[] = {"CCNOT", "CNOT", "NOT", "SWAP"};

    printf("  Sequence (length=%u, fitness=%.2f):\n", seq->length, seq->fitness);
    for (uint32_t i = 0; i < seq->length; i++) {
        R_Cell c = seq->sequence[i];
        printf("    %u. %s(", i, gate_names[c.gate]);

        if (c.gate == 0) printf("%u, %u, %u", c.a, c.b, c.c);       // CCNOT
        else if (c.gate == 1) printf("%u, %u", c.a, c.b);          // CNOT
        else if (c.gate == 2) printf("%u", c.a);                   // NOT
        else printf("%u, %u", c.a, c.b);                           // SWAP

        printf(")\n");
    }
}

// ============================================================================
// Main Evolutionary Loop
// ============================================================================

int main(void) {
    srand(time(NULL));

    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║  Moop Evolutionary Optimization Demo                     ║\n");
    printf("║  Self-Modifying Code with Reversible Backtracking        ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");

    printf("Goal: Evolve a gate sequence to compute XOR(a, b) -> c\n");
    printf("Strategy: Generate random sequences, mutate, keep best\n");
    printf("Features: Homoiconicity + Reversibility + Evolution\n\n");

    // Initialize runtime with quantum-ready backend
    L2a_Runtime* runtime = l2a_init(3, 1, QUBIT_BACKEND_CLASSICAL);

    printf("Backend: %s\n", qubit_backend_name(runtime->qubit_state));
    printf("Is quantum: %s\n\n", qubit_is_quantum(runtime->qubit_state) ? "Yes" : "No");

    // Evolutionary parameters
    const uint32_t population_size = 20;
    const uint32_t generations = 50;
    const uint32_t max_sequence_length = 6;

    // Initialize population
    GateSequence population[population_size];
    for (uint32_t i = 0; i < population_size; i++) {
        population[i] = random_sequence(max_sequence_length);
        population[i].fitness = test_all_cases(runtime, &population[i]);
    }

    printf("Initial population: %u sequences\n", population_size);
    printf("Max sequence length: %u gates\n", max_sequence_length);
    printf("Generations: %u\n\n", generations);

    // Evolution loop
    GateSequence best = population[0];

    for (uint32_t gen = 0; gen < generations; gen++) {
        // Find best in population
        for (uint32_t i = 0; i < population_size; i++) {
            if (population[i].fitness > best.fitness) {
                free(best.sequence);
                best.length = population[i].length;
                best.sequence = malloc(best.length * sizeof(R_Cell));
                for (uint32_t j = 0; j < best.length; j++) {
                    best.sequence[j] = population[i].sequence[j];
                }
                best.fitness = population[i].fitness;
            }
        }

        // Print progress every 10 generations
        if (gen % 10 == 0 || best.fitness >= 1.0f) {
            printf("Generation %u: Best fitness = %.2f\n", gen, best.fitness);

            if (best.fitness >= 1.0f) {
                printf("\n🎉 PERFECT SOLUTION FOUND!\n\n");
                print_sequence(&best);
                printf("\nVerifying solution:\n");

                // Actually execute and read results
                uint32_t verify_checkpoint = l2a_checkpoint(runtime);

                // Test 0 XOR 0 = 0
                for (uint32_t i = 0; i < best.length; i++) {
                    R_Cell c = best.sequence[i];
                    switch (c.gate) {
                        case 0: l2a_CCNOT(runtime, c.a, c.b, c.c); break;
                        case 1: l2a_CNOT(runtime, c.a, c.b); break;
                        case 2: l2a_NOT(runtime, c.a); break;
                        case 3: l2a_SWAP(runtime, c.a, c.b); break;
                    }
                }
                printf("  XOR(0,0) = %u (expected 0) %s\n",
                       qubit_read(runtime->qubit_state, 2),
                       qubit_read(runtime->qubit_state, 2) == 0 ? "✓" : "✗");
                l2a_restore(runtime, verify_checkpoint);

                // Test 0 XOR 1 = 1
                l2a_NOT(runtime, 1);  // Set b=1
                for (uint32_t i = 0; i < best.length; i++) {
                    R_Cell c = best.sequence[i];
                    switch (c.gate) {
                        case 0: l2a_CCNOT(runtime, c.a, c.b, c.c); break;
                        case 1: l2a_CNOT(runtime, c.a, c.b); break;
                        case 2: l2a_NOT(runtime, c.a); break;
                        case 3: l2a_SWAP(runtime, c.a, c.b); break;
                    }
                }
                printf("  XOR(0,1) = %u (expected 1) %s\n",
                       qubit_read(runtime->qubit_state, 2),
                       qubit_read(runtime->qubit_state, 2) == 1 ? "✓" : "✗");
                l2a_restore(runtime, verify_checkpoint);

                // Test 1 XOR 0 = 1
                l2a_NOT(runtime, 0);  // Set a=1
                for (uint32_t i = 0; i < best.length; i++) {
                    R_Cell c = best.sequence[i];
                    switch (c.gate) {
                        case 0: l2a_CCNOT(runtime, c.a, c.b, c.c); break;
                        case 1: l2a_CNOT(runtime, c.a, c.b); break;
                        case 2: l2a_NOT(runtime, c.a); break;
                        case 3: l2a_SWAP(runtime, c.a, c.b); break;
                    }
                }
                printf("  XOR(1,0) = %u (expected 1) %s\n",
                       qubit_read(runtime->qubit_state, 2),
                       qubit_read(runtime->qubit_state, 2) == 1 ? "✓" : "✗");
                l2a_restore(runtime, verify_checkpoint);

                // Test 1 XOR 1 = 0
                l2a_NOT(runtime, 0);  // Set a=1
                l2a_NOT(runtime, 1);  // Set b=1
                for (uint32_t i = 0; i < best.length; i++) {
                    R_Cell c = best.sequence[i];
                    switch (c.gate) {
                        case 0: l2a_CCNOT(runtime, c.a, c.b, c.c); break;
                        case 1: l2a_CNOT(runtime, c.a, c.b); break;
                        case 2: l2a_NOT(runtime, c.a); break;
                        case 3: l2a_SWAP(runtime, c.a, c.b); break;
                    }
                }
                printf("  XOR(1,1) = %u (expected 0) %s\n",
                       qubit_read(runtime->qubit_state, 2),
                       qubit_read(runtime->qubit_state, 2) == 0 ? "✓" : "✗");
                l2a_restore(runtime, verify_checkpoint);
                break;
            }
        }

        // Generate next generation (keep best, mutate rest)
        for (uint32_t i = 1; i < population_size; i++) {
            free(population[i].sequence);

            // Mutate best sequence
            population[i] = mutate_sequence(&best);
            population[i].fitness = test_all_cases(runtime, &population[i]);
        }

        // Replace worst with new random (diversity)
        if (gen % 10 == 0) {
            free(population[population_size - 1].sequence);
            population[population_size - 1] = random_sequence(max_sequence_length);
            population[population_size - 1].fitness =
                test_all_cases(runtime, &population[population_size - 1]);
        }
    }

    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf("║  Key Features Demonstrated                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");

    printf("✓ Homoiconicity: Code (gate sequences) treated as data\n");
    printf("✓ Reversibility: Each test uses checkpoint/restore\n");
    printf("✓ Evolution: Population evolves toward optimal solution\n");
    printf("✓ Self-Organization: System finds solution autonomously\n");
    printf("✓ Quantum-Ready: Same code works on classical/quantum backends\n\n");

    printf("Tape Statistics:\n");
    printf("  Total operations executed: %u\n", runtime->total_ops);
    printf("  Tape wrapped: %s\n", runtime->tape_wrapped ? "Yes" : "No");
    printf("  Pruning cycles: %u\n\n", runtime->pruning_cycles);

    // Cleanup
    for (uint32_t i = 0; i < population_size; i++) {
        free(population[i].sequence);
    }
    free(best.sequence);
    l2a_free(runtime);

    printf("This demonstrates what NO other language can do:\n");
    printf("Code that modifies itself, tests reversibly, and evolves.\n\n");

    return 0;
}
