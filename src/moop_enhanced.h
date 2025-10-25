// moop_enhanced.h
// Enhanced Unified Moop with Tape-Loop, Trinary MAYBE, Self-Modification
// Implements all documented architectural features

#ifndef MOOP_ENHANCED_H
#define MOOP_ENHANCED_H

#include <stdint.h>
#include <stdbool.h>

// ============================================================================
// L1/L2a: Tape-Loop Turing Machine (Enhanced)
// ============================================================================

#define L1_TAPE_SIZE 1024  // From documentation: L1 limited to 1024 operations

// R_Cell unchanged (4 bytes)
typedef struct {
    uint8_t gate;      // 0=CCNOT, 1=CNOT, 2=NOT, 3=SWAP
    uint8_t a, b, c;
} __attribute__((packed)) R_Cell;

// Enhanced tape entry with evolutionary fitness (Enhancement 5)
typedef struct {
    R_Cell cell;           // The operation
    float fitness;         // Evolutionary fitness (0.0-1.0)
    uint32_t last_used;    // Recency (for LRU component)
    bool essential;        // Marked as essential (never prune)
} Tape_Entry;

// Meta-evolution parameters
typedef struct {
    float recency_weight;      // Weight for recency component (default 0.5)
    float activity_weight;     // Weight for qubit activity (default 0.3)
    float gate_weight;         // Weight for gate priority (default 0.2)
    uint32_t prune_interval;   // Operations between pruning (default 256)
    float prune_threshold;     // Fraction to keep (default 0.75)
} Fitness_Params;

// Enhanced L2a Runtime: Tape-Loop with evolutionary pruning
typedef struct {
    uint8_t* qubits;           // Qubit states
    Tape_Entry* tape;          // Circular tape with fitness (1024 entries)
    uint32_t tape_head;        // Current position (wraps)
    uint32_t qubit_count;
    uint32_t instance_id;

    // Tape-loop metadata
    uint32_t total_ops;        // Total ops executed (can exceed 1024)
    bool tape_wrapped;         // Has tape wrapped around?

    // Evolutionary pruning metadata
    uint32_t pruning_cycles;   // Number of pruning cycles executed
    uint32_t last_prune_op;    // Operation count at last pruning

    // Meta-evolution parameters (adaptive fitness tuning)
    Fitness_Params fitness_params;
} L2a_Runtime;

// L2a API (unchanged)
L2a_Runtime* l2a_init(uint32_t qubits, uint32_t instance_id);
void l2a_free(L2a_Runtime* r);

void l2a_CCNOT(L2a_Runtime* r, uint8_t a, uint8_t b, uint8_t c);
void l2a_CNOT(L2a_Runtime* r, uint8_t a, uint8_t b);
void l2a_NOT(L2a_Runtime* r, uint8_t a);
void l2a_SWAP(L2a_Runtime* r, uint8_t a, uint8_t b);

uint32_t l2a_checkpoint(L2a_Runtime* r);
void l2a_restore(L2a_Runtime* r, uint32_t checkpoint);

const char* l2a_print(R_Cell cell);

// ============================================================================
// Self-Modification API (NEW)
// ============================================================================

// Read a cell from the tape (homoiconic read)
R_Cell l2a_read_tape(L2a_Runtime* r, uint32_t index);

// Write a cell to the tape (homoiconic write)
void l2a_write_tape(L2a_Runtime* r, uint32_t index, R_Cell cell);

// Meta-modify: Apply a modification rule to the tape itself
void l2a_meta_modify(L2a_Runtime* r, R_Cell* modification_rule, uint32_t rule_len);

// ============================================================================
// Evolutionary Pruning API (NEW - Enhancement 5)
// ============================================================================

// Compute fitness for a tape entry based on current execution state
float l2a_compute_fitness(L2a_Runtime* r, uint32_t index);

// Mark operation as essential (never prune, e.g., checkpoints)
void l2a_mark_essential(L2a_Runtime* r, uint32_t index);

// Perform evolutionary pruning cycle (selective retention)
void l2a_prune_tape(L2a_Runtime* r);

// Get tape entry with fitness metadata
Tape_Entry l2a_get_tape_entry(L2a_Runtime* r, uint32_t index);

// Tape statistics and introspection
typedef struct {
    float avg_fitness;         // Average fitness across tape
    float min_fitness;         // Lowest fitness entry
    float max_fitness;         // Highest fitness entry
    uint32_t essential_count;  // Number of essential entries
    uint32_t active_count;     // Number of non-zero entries
    uint32_t pruning_cycles;   // Total pruning cycles executed
} Tape_Stats;

Tape_Stats l2a_get_tape_stats(L2a_Runtime* r);

// Get current fitness parameters
Fitness_Params l2a_get_fitness_params(L2a_Runtime* r);

// Tune fitness parameters (meta-evolution)
void l2a_tune_fitness(L2a_Runtime* r, Fitness_Params params);

// ============================================================================
// L2b: Enhanced with Trinary MAYBE and Entropy Tracking
// ============================================================================

// Trinary MAYBE states (NEW)
typedef enum {
    MAYBE_FALSE = 0,
    MAYBE_TRUE = 1,
    MAYBE_UNRESOLVED = 2
} MaybeState;

// Enhanced MAYBE with LLM context (NEW)
typedef struct {
    MaybeState state;
    char* condition_name;
    float confidence;          // LLM confidence score (0.0-1.0)
    char* llm_reasoning;       // LLM explanation
    void* context_data;        // Additional context for LLM
} L2b_Maybe;

typedef struct {
    L2a_Runtime* l2a;
} L2b_Runtime;

// L2b API
L2b_Runtime* l2b_init(L2a_Runtime* l2a);
void l2b_free(L2b_Runtime* r);

// Irreversible operations
void l2b_AND(L2b_Runtime* r, uint8_t a, uint8_t b, uint8_t result);
void l2b_OR(L2b_Runtime* r, uint8_t a, uint8_t b, uint8_t result);
void l2b_XOR(L2b_Runtime* r, uint8_t a, uint8_t b, uint8_t result);
void l2b_NAND(L2b_Runtime* r, uint8_t a, uint8_t b, uint8_t result);
void l2b_NOR(L2b_Runtime* r, uint8_t a, uint8_t b, uint8_t result);

// Enhanced MAYBE API (NEW)
L2b_Maybe l2b_maybe_create(const char* condition_name);
void l2b_maybe_resolve(L2b_Maybe* m, bool value, float confidence, const char* reasoning);
bool l2b_maybe_is_resolved(L2b_Maybe* m);
MaybeState l2b_maybe_get_state(L2b_Maybe* m);
void l2b_maybe_free(L2b_Maybe* m);

// ============================================================================
// L2c, L3a, L3b: Unchanged from moop_refined.h
// ============================================================================

typedef struct L3_Actor L3_Actor;
typedef struct L3_Proto L3_Proto;

struct L3_Proto {
    const char* name;
    L3_Proto* parent;
    void* slots;
    uint32_t slot_count;
};

struct L3_Actor {
    const char* name;
    const char* role;
    L3_Proto* prototype;
    void* state;
    uint32_t state_size;
};

typedef struct {
    L2a_Runtime* l2a;
    L2b_Runtime* l2b;
    L3_Actor* root_actor;
    L3_Proto* root_proto;
    uint32_t instance_id;
} L3a_Runtime;

typedef struct {
    L3a_Runtime* l3a;
    L3_Actor** actors;
    L3_Proto** protos;
    uint32_t actor_count;
    uint32_t proto_count;
} L3b_Runtime;

L3a_Runtime* l3a_init(L2a_Runtime* l2a, L2b_Runtime* l2b, uint32_t instance_id);
void l3a_free(L3a_Runtime* r);
void l3a_bootstrap_dual(L3a_Runtime* r);

L3b_Runtime* l3b_init(L3a_Runtime* l3a);
void l3b_free(L3b_Runtime* r);

L3_Actor* l3b_create_actor(L3b_Runtime* r, const char* name, const char* role);
L3_Proto* l3b_create_proto(L3b_Runtime* r, const char* name, L3_Proto* parent);
void l3b_send_message(L3_Actor* actor, const char* msg);

// ============================================================================
// Natural Language Parser (NEW)
// ============================================================================

typedef struct {
    char* source;          // Source code
    uint32_t length;
} NL_Source;

typedef struct {
    L3b_Runtime* l3b;
    NL_Source* source;
} NL_Parser;

// Parse natural language actor definition
L3_Actor* nl_parse_actor(NL_Parser* parser);

// Parse natural language proto definition
L3_Proto* nl_parse_proto(NL_Parser* parser);

// Helper: Check if line starts with keyword
bool nl_starts_with(const char* line, const char* keyword);

// Helper: Extract value after "is"
const char* nl_extract_value(const char* line);

// ============================================================================
// Unified Runtime (All Layers)
// ============================================================================

typedef struct {
    uint32_t instance_id;
    L2a_Runtime* l2a;
    L2b_Runtime* l2b;
    L3a_Runtime* l3a;
    L3b_Runtime* l3b;
} Moop_Runtime;

Moop_Runtime* moop_init(uint32_t qubits, uint32_t instance_id);
void moop_free(Moop_Runtime* moop);

// Introspection API (NEW)
void moop_print_stats(Moop_Runtime* moop);

#endif // MOOP_ENHANCED_H
