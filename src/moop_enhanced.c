// moop_enhanced.c
// Enhanced Unified Moop Implementation (~450 lines with all improvements)

#include "moop_enhanced.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ============================================================================
// L2a: Tape-Loop Turing Machine (Enhancement 1)
// ============================================================================

L2a_Runtime* l2a_init(uint32_t qubits, uint32_t instance_id) {
    L2a_Runtime* r = malloc(sizeof(L2a_Runtime));
    if (!r) return NULL;

    r->qubits = calloc(qubits, 1);
    if (!r->qubits) {
        free(r);
        return NULL;
    }

    r->tape = malloc(L1_TAPE_SIZE * sizeof(Tape_Entry));
    if (!r->tape) {
        free(r->qubits);
        free(r);
        return NULL;
    }

    // Initialize tape entries with zero fitness
    for (uint32_t i = 0; i < L1_TAPE_SIZE; i++) {
        r->tape[i].cell = (R_Cell){0, 0, 0, 0};
        r->tape[i].fitness = 0.0f;
        r->tape[i].last_used = 0;
        r->tape[i].essential = false;
    }

    r->qubit_count = qubits;
    r->tape_head = 0;
    r->instance_id = instance_id;
    r->total_ops = 0;
    r->tape_wrapped = false;
    r->pruning_cycles = 0;
    r->last_prune_op = 0;

    // Initialize default fitness parameters
    r->fitness_params.recency_weight = 0.5f;
    r->fitness_params.activity_weight = 0.3f;
    r->fitness_params.gate_weight = 0.2f;
    r->fitness_params.prune_interval = 256;
    r->fitness_params.prune_threshold = 0.75f;

    return r;
}

void l2a_free(L2a_Runtime* r) {
    free(r->qubits);
    free(r->tape);
    free(r);
}

// Helper: Record operation to circular tape with evolutionary pruning
static void record_to_tape(L2a_Runtime* r, R_Cell cell) {
    uint32_t target_index = r->tape_head;

    // Compute fitness for new operation
    float new_fitness = l2a_compute_fitness(r, target_index);

    // Check if existing entry should be protected
    Tape_Entry* existing = &r->tape[target_index];

    // Evolutionary selection: only overwrite if new op has higher fitness
    // OR if existing entry is not essential
    if (!existing->essential && (new_fitness >= existing->fitness || !r->tape_wrapped)) {
        r->tape[target_index].cell = cell;
        r->tape[target_index].fitness = new_fitness;
        r->tape[target_index].last_used = r->total_ops;
        r->tape[target_index].essential = false;
    } else if (new_fitness < existing->fitness && r->tape_wrapped) {
        // Skip recording (pruned) - low fitness operation discarded
        return;
    }

    r->tape_head = (r->tape_head + 1) % L1_TAPE_SIZE;  // Wrap around
    r->total_ops++;

    if (r->tape_head == 0 && r->total_ops > 0) {
        r->tape_wrapped = true;  // Tape has wrapped
    }

    // Trigger evolutionary pruning based on adaptive interval
    if (r->total_ops - r->last_prune_op >= r->fitness_params.prune_interval) {
        l2a_prune_tape(r);
    }
}

// The 4 reversible primitives (with tape recording)

void l2a_CCNOT(L2a_Runtime* r, uint8_t a, uint8_t b, uint8_t c) {
    if (r->qubits[a] && r->qubits[b]) {
        r->qubits[c] ^= 1;
    }
    record_to_tape(r, (R_Cell){0, a, b, c});
}

void l2a_CNOT(L2a_Runtime* r, uint8_t a, uint8_t b) {
    if (r->qubits[a]) {
        r->qubits[b] ^= 1;
    }
    record_to_tape(r, (R_Cell){1, a, b, 0});
}

void l2a_NOT(L2a_Runtime* r, uint8_t a) {
    r->qubits[a] ^= 1;
    record_to_tape(r, (R_Cell){2, a, 0, 0});
}

void l2a_SWAP(L2a_Runtime* r, uint8_t a, uint8_t b) {
    uint8_t t = r->qubits[a];
    r->qubits[a] = r->qubits[b];
    r->qubits[b] = t;
    record_to_tape(r, (R_Cell){3, a, b, 0});
}

// Reversibility (tape-loop aware)
uint32_t l2a_checkpoint(L2a_Runtime* r) {
    uint32_t checkpoint_pos = r->tape_head;

    // Mark checkpoint as essential (never prune)
    l2a_mark_essential(r, checkpoint_pos);

    return checkpoint_pos;  // Return current tape position
}

void l2a_restore(L2a_Runtime* r, uint32_t checkpoint) {
    // Rewind tape head to checkpoint
    while (r->tape_head != checkpoint) {
        // Move backward
        r->tape_head = (r->tape_head == 0) ? L1_TAPE_SIZE - 1 : r->tape_head - 1;

        // Execute inverse
        R_Cell c = r->tape[r->tape_head].cell;
        switch(c.gate) {
            case 0: if (r->qubits[c.a] && r->qubits[c.b]) r->qubits[c.c] ^= 1; break;
            case 1: if (r->qubits[c.a]) r->qubits[c.b] ^= 1; break;
            case 2: r->qubits[c.a] ^= 1; break;
            case 3: { uint8_t t = r->qubits[c.a]; r->qubits[c.a] = r->qubits[c.b]; r->qubits[c.b] = t; } break;
        }

        r->total_ops--;
    }
}

const char* l2a_print(R_Cell c) {
    static char buf[64];
    const char* gates[] = {"CCNOT", "CNOT", "NOT", "SWAP"};
    sprintf(buf, "%s %d %d %d", gates[c.gate], c.a, c.b, c.c);
    return buf;
}

// ============================================================================
// Self-Modification API (Enhancement 3)
// ============================================================================

R_Cell l2a_read_tape(L2a_Runtime* r, uint32_t index) {
    return r->tape[index % L1_TAPE_SIZE].cell;
}

void l2a_write_tape(L2a_Runtime* r, uint32_t index, R_Cell cell) {
    r->tape[index % L1_TAPE_SIZE].cell = cell;
    r->tape[index % L1_TAPE_SIZE].last_used = r->total_ops;
}

void l2a_meta_modify(L2a_Runtime* r, R_Cell* modification_rule, uint32_t rule_len) {
    // Apply modification rule to tape itself
    for (uint32_t i = 0; i < rule_len; i++) {
        R_Cell rule = modification_rule[i];
        // Interpret rule as: "modify tape cell at position rule.a"
        if (rule.gate == 0) {  // CCNOT used as modify instruction
            R_Cell target = r->tape[rule.a].cell;
            target.gate = rule.b;  // Change gate type
            r->tape[rule.a].cell = target;
            r->tape[rule.a].last_used = r->total_ops;
        }
    }
}

// ============================================================================
// Evolutionary Pruning API (Enhancement 5)
// ============================================================================

// Fitness function: evaluates relevance of tape entry based on:
// 1. Recency (LRU component)
// 2. Qubit dependency (operations on "active" qubits)
// 3. Gate type (some operations more fundamental than others)
float l2a_compute_fitness(L2a_Runtime* r, uint32_t index) {
    Tape_Entry* entry = &r->tape[index];

    // Component 1: Recency (0.0-1.0, exponential decay)
    uint32_t age = r->total_ops - entry->last_used;
    float recency = (age == 0) ? 1.0f : (1.0f / (1.0f + age / 100.0f));

    // Component 2: Qubit dependency (operations on non-zero qubits are "hotter")
    float qubit_activity = 0.0f;
    if (entry->cell.a < r->qubit_count && r->qubits[entry->cell.a]) qubit_activity += 0.3f;
    if (entry->cell.b < r->qubit_count && r->qubits[entry->cell.b]) qubit_activity += 0.3f;
    if (entry->cell.c < r->qubit_count && r->qubits[entry->cell.c]) qubit_activity += 0.2f;

    // Component 3: Gate type priority (CCNOT > CNOT > SWAP > NOT)
    float gate_priority = 0.0f;
    switch (entry->cell.gate) {
        case 0: gate_priority = 0.4f; break;  // CCNOT (universal gate)
        case 1: gate_priority = 0.3f; break;  // CNOT
        case 3: gate_priority = 0.2f; break;  // SWAP
        case 2: gate_priority = 0.1f; break;  // NOT
    }

    // Weighted sum: use adaptive fitness parameters
    float fitness = r->fitness_params.recency_weight * recency +
                    r->fitness_params.activity_weight * qubit_activity +
                    r->fitness_params.gate_weight * gate_priority;

    // Essential entries get max fitness (never pruned)
    if (entry->essential) {
        fitness = 1.0f;
    }

    return fitness;
}

void l2a_mark_essential(L2a_Runtime* r, uint32_t index) {
    r->tape[index % L1_TAPE_SIZE].essential = true;
    r->tape[index % L1_TAPE_SIZE].fitness = 1.0f;
}

void l2a_prune_tape(L2a_Runtime* r) {
    // Evolutionary pruning: compact high-fitness entries, discard low-fitness

    // 1. Recompute fitness for all entries
    for (uint32_t i = 0; i < L1_TAPE_SIZE; i++) {
        if (!r->tape[i].essential) {
            r->tape[i].fitness = l2a_compute_fitness(r, i);
        }
    }

    // 2. Sort entries by fitness (selection pressure)
    // For simplicity: bubble sort (good enough for 1024 entries)
    for (uint32_t i = 0; i < L1_TAPE_SIZE - 1; i++) {
        for (uint32_t j = 0; j < L1_TAPE_SIZE - i - 1; j++) {
            if (r->tape[j].fitness < r->tape[j + 1].fitness) {
                Tape_Entry temp = r->tape[j];
                r->tape[j] = r->tape[j + 1];
                r->tape[j + 1] = temp;
            }
        }
    }

    // 3. Reset low-fitness entries based on adaptive threshold
    uint32_t prune_threshold = (uint32_t)(L1_TAPE_SIZE * r->fitness_params.prune_threshold);
    for (uint32_t i = prune_threshold; i < L1_TAPE_SIZE; i++) {
        if (!r->tape[i].essential) {
            r->tape[i].cell = (R_Cell){0, 0, 0, 0};
            r->tape[i].fitness = 0.0f;
            r->tape[i].last_used = 0;
        }
    }

    r->pruning_cycles++;
    r->last_prune_op = r->total_ops;
}

Tape_Entry l2a_get_tape_entry(L2a_Runtime* r, uint32_t index) {
    return r->tape[index % L1_TAPE_SIZE];
}

// Tape statistics for introspection and meta-evolution
Tape_Stats l2a_get_tape_stats(L2a_Runtime* r) {
    Tape_Stats stats = {0};
    float fitness_sum = 0.0f;
    stats.min_fitness = 1.0f;
    stats.max_fitness = 0.0f;

    for (uint32_t i = 0; i < L1_TAPE_SIZE; i++) {
        Tape_Entry* entry = &r->tape[i];

        // Count active entries (non-zero gate)
        if (entry->cell.gate != 0 || entry->cell.a != 0) {
            stats.active_count++;
        }

        // Count essential entries
        if (entry->essential) {
            stats.essential_count++;
        }

        // Fitness statistics
        fitness_sum += entry->fitness;
        if (entry->fitness < stats.min_fitness) stats.min_fitness = entry->fitness;
        if (entry->fitness > stats.max_fitness) stats.max_fitness = entry->fitness;
    }

    stats.avg_fitness = fitness_sum / L1_TAPE_SIZE;
    stats.pruning_cycles = r->pruning_cycles;

    return stats;
}

// Meta-evolution: Get current fitness parameters
Fitness_Params l2a_get_fitness_params(L2a_Runtime* r) {
    return r->fitness_params;
}

// Meta-evolution: Tune fitness parameters (evolving the evolution)
void l2a_tune_fitness(L2a_Runtime* r, Fitness_Params params) {
    // Normalize weights to sum to 1.0
    float total_weight = params.recency_weight + params.activity_weight + params.gate_weight;
    if (total_weight > 0.0f) {
        r->fitness_params.recency_weight = params.recency_weight / total_weight;
        r->fitness_params.activity_weight = params.activity_weight / total_weight;
        r->fitness_params.gate_weight = params.gate_weight / total_weight;
    }

    // Update pruning parameters
    if (params.prune_interval > 0) {
        r->fitness_params.prune_interval = params.prune_interval;
    }

    if (params.prune_threshold > 0.0f && params.prune_threshold <= 1.0f) {
        r->fitness_params.prune_threshold = params.prune_threshold;
    }

    // Recompute fitness for all entries with new parameters
    for (uint32_t i = 0; i < L1_TAPE_SIZE; i++) {
        if (!r->tape[i].essential) {
            r->tape[i].fitness = l2a_compute_fitness(r, i);
        }
    }
}

// ============================================================================
// L2b: Enhanced with Trinary MAYBE (Enhancement 2)
// ============================================================================

L2b_Runtime* l2b_init(L2a_Runtime* l2a) {
    L2b_Runtime* r = malloc(sizeof(L2b_Runtime));
    if (!r) return NULL;
    r->l2a = l2a;
    return r;
}

void l2b_free(L2b_Runtime* r) {
    free(r);
}

// Irreversible operations

void l2b_AND(L2b_Runtime* r, uint8_t a, uint8_t b, uint8_t result) {
    if (r->l2a->qubits[result]) l2a_NOT(r->l2a, result);
    l2a_CCNOT(r->l2a, a, b, result);
}

void l2b_OR(L2b_Runtime* r, uint8_t a, uint8_t b, uint8_t result) {
    l2a_NOT(r->l2a, a);
    l2a_NOT(r->l2a, b);
    l2b_AND(r, a, b, result);
    l2a_NOT(r->l2a, result);
    l2a_NOT(r->l2a, a);
    l2a_NOT(r->l2a, b);
}

void l2b_XOR(L2b_Runtime* r, uint8_t a, uint8_t b, uint8_t result) {
    if (r->l2a->qubits[result]) l2a_NOT(r->l2a, result);
    l2a_CNOT(r->l2a, a, result);
    l2a_CNOT(r->l2a, b, result);
}

void l2b_NAND(L2b_Runtime* r, uint8_t a, uint8_t b, uint8_t result) {
    l2b_AND(r, a, b, result);
    l2a_NOT(r->l2a, result);
}

void l2b_NOR(L2b_Runtime* r, uint8_t a, uint8_t b, uint8_t result) {
    l2b_OR(r, a, b, result);
    l2a_NOT(r->l2a, result);
}

// Enhanced MAYBE API (Trinary)

L2b_Maybe l2b_maybe_create(const char* condition_name) {
    L2b_Maybe m = {
        .state = MAYBE_UNRESOLVED,
        .condition_name = strdup(condition_name),
        .confidence = 0.0f,
        .llm_reasoning = NULL,
        .context_data = NULL
    };
    return m;
}

void l2b_maybe_resolve(L2b_Maybe* m, bool value, float confidence, const char* reasoning) {
    m->state = value ? MAYBE_TRUE : MAYBE_FALSE;
    m->confidence = confidence;
    if (reasoning) {
        m->llm_reasoning = strdup(reasoning);
    }
}

bool l2b_maybe_is_resolved(L2b_Maybe* m) {
    return m->state != MAYBE_UNRESOLVED;
}

MaybeState l2b_maybe_get_state(L2b_Maybe* m) {
    return m->state;
}

void l2b_maybe_free(L2b_Maybe* m) {
    free(m->condition_name);
    free(m->llm_reasoning);
}

// ============================================================================
// L3a: System Processing (Bootstrap Coordination)
// ============================================================================

L3a_Runtime* l3a_init(L2a_Runtime* l2a, L2b_Runtime* l2b, uint32_t instance_id) {
    L3a_Runtime* r = malloc(sizeof(L3a_Runtime));
    if (!r) return NULL;
    r->l2a = l2a;
    r->l2b = l2b;
    r->instance_id = instance_id;
    r->root_actor = NULL;
    r->root_proto = NULL;
    return r;
}

void l3a_free(L3a_Runtime* r) {
    if (r->root_proto) {
        free(r->root_proto->slots);
        free(r->root_proto);
    }
    if (r->root_actor) {
        free(r->root_actor->state);
        free(r->root_actor);
    }
    free(r);
}

void l3a_bootstrap_dual(L3a_Runtime* r) {
    // Create root_proto (maximum binding)
    r->root_proto = malloc(sizeof(L3_Proto));
    if (!r->root_proto) return;
    r->root_proto->name = "root_proto";
    r->root_proto->parent = NULL;
    r->root_proto->slots = NULL;
    r->root_proto->slot_count = 0;

    // Create root_actor (minimal binding)
    r->root_actor = malloc(sizeof(L3_Actor));
    if (!r->root_actor) {
        free(r->root_proto);
        r->root_proto = NULL;
        return;
    }
    r->root_actor->name = "root_actor";
    r->root_actor->role = "Bootstrap temporal coordinator";
    r->root_actor->prototype = r->root_proto;
    r->root_actor->state = NULL;
    r->root_actor->state_size = 0;
}

// ============================================================================
// L3b: User Processing (User Actors/Protos)
// ============================================================================

L3b_Runtime* l3b_init(L3a_Runtime* l3a) {
    L3b_Runtime* r = malloc(sizeof(L3b_Runtime));
    if (!r) return NULL;

    r->actors = malloc(256 * sizeof(L3_Actor*));
    if (!r->actors) {
        free(r);
        return NULL;
    }

    r->protos = malloc(256 * sizeof(L3_Proto*));
    if (!r->protos) {
        free(r->actors);
        free(r);
        return NULL;
    }

    r->l3a = l3a;
    r->actor_count = 0;
    r->proto_count = 0;
    return r;
}

void l3b_free(L3b_Runtime* r) {
    for (uint32_t i = 0; i < r->actor_count; i++) {
        free(r->actors[i]->state);
        free(r->actors[i]);
    }
    for (uint32_t i = 0; i < r->proto_count; i++) {
        free(r->protos[i]->slots);
        free(r->protos[i]);
    }
    free(r->actors);
    free(r->protos);
    free(r);
}

L3_Actor* l3b_create_actor(L3b_Runtime* r, const char* name, const char* role) {
    L3_Actor* actor = malloc(sizeof(L3_Actor));
    if (!actor) return NULL;
    actor->name = name;
    actor->role = role;
    actor->prototype = r->l3a->root_proto;
    actor->state = NULL;
    actor->state_size = 0;
    r->actors[r->actor_count++] = actor;
    return actor;
}

L3_Proto* l3b_create_proto(L3b_Runtime* r, const char* name, L3_Proto* parent) {
    L3_Proto* proto = malloc(sizeof(L3_Proto));
    if (!proto) return NULL;
    proto->name = name;
    proto->parent = parent ? parent : r->l3a->root_proto;
    proto->slots = NULL;
    proto->slot_count = 0;
    r->protos[r->proto_count++] = proto;
    return proto;
}

void l3b_send_message(L3_Actor* actor, const char* msg) {
    printf("Actor '%s' received: %s\n", actor->name, msg);
}

// ============================================================================
// Natural Language Parser (Enhancement 4)
// ============================================================================

bool nl_starts_with(const char* line, const char* keyword) {
    return strncmp(line, keyword, strlen(keyword)) == 0;
}

const char* nl_extract_value(const char* line) {
    const char* is_pos = strstr(line, " is ");
    if (is_pos) {
        is_pos += 4;  // Skip " is "
        // Trim quotes if present
        if (*is_pos == '"') {
            is_pos++;
            static char buf[256];
            size_t len = strcspn(is_pos, "\"");
            strncpy(buf, is_pos, len);
            buf[len] = '\0';
            return buf;
        }
        return is_pos;
    }
    return NULL;
}

L3_Actor* nl_parse_actor(NL_Parser* parser) {
    // Simple parser for:
    // actor <Name>
    //     role is "<description>"
    //     state has
    //         <field> is <value>

    char* line = strtok(parser->source->source, "\n");
    const char* name = NULL;
    const char* role = NULL;

    while (line) {
        // Trim leading whitespace
        while (*line == ' ' || *line == '\t') line++;

        if (nl_starts_with(line, "actor ")) {
            name = line + 6;  // Skip "actor "
        } else if (nl_starts_with(line, "role is")) {
            role = nl_extract_value(line);
        }

        line = strtok(NULL, "\n");
    }

    if (name && role) {
        return l3b_create_actor(parser->l3b, name, role);
    }

    return NULL;
}

L3_Proto* nl_parse_proto(NL_Parser* parser) {
    // Simple parser for:
    // proto <Name> <- <Parent>
    //     slots: <field1>, <field2>

    char* line = strtok(parser->source->source, "\n");
    const char* name = NULL;

    while (line) {
        while (*line == ' ' || *line == '\t') line++;

        if (nl_starts_with(line, "proto ")) {
            name = line + 6;  // Skip "proto "
            // Parse name before "<-"
            char* arrow = strstr(line, " <-");
            if (arrow) {
                static char proto_name[256];
                size_t len = arrow - (line + 6);
                strncpy(proto_name, line + 6, len);
                proto_name[len] = '\0';
                name = proto_name;
            }
        }

        line = strtok(NULL, "\n");
    }

    if (name) {
        return l3b_create_proto(parser->l3b, name, NULL);
    }

    return NULL;
}

// ============================================================================
// Unified Runtime
// ============================================================================

Moop_Runtime* moop_init(uint32_t qubits, uint32_t instance_id) {
    Moop_Runtime* moop = malloc(sizeof(Moop_Runtime));
    if (!moop) return NULL;
    moop->instance_id = instance_id;

    moop->l2a = l2a_init(qubits, instance_id);
    if (!moop->l2a) {
        free(moop);
        return NULL;
    }

    moop->l2b = l2b_init(moop->l2a);
    if (!moop->l2b) {
        l2a_free(moop->l2a);
        free(moop);
        return NULL;
    }

    moop->l3a = l3a_init(moop->l2a, moop->l2b, instance_id);
    if (!moop->l3a) {
        l2b_free(moop->l2b);
        l2a_free(moop->l2a);
        free(moop);
        return NULL;
    }

    l3a_bootstrap_dual(moop->l3a);

    moop->l3b = l3b_init(moop->l3a);
    if (!moop->l3b) {
        l3a_free(moop->l3a);
        l2b_free(moop->l2b);
        l2a_free(moop->l2a);
        free(moop);
        return NULL;
    }

    return moop;
}

void moop_free(Moop_Runtime* moop) {
    l3b_free(moop->l3b);
    l3a_free(moop->l3a);
    l2b_free(moop->l2b);
    l2a_free(moop->l2a);
    free(moop);
}

void moop_print_stats(Moop_Runtime* moop) {
    printf("=== Moop Runtime Statistics ===\n");
    printf("Instance ID: %u\n", moop->instance_id);
    printf("Qubits: %u\n", moop->l2a->qubit_count);
    printf("Tape size: %d cells\n", L1_TAPE_SIZE);
    printf("Tape head: %u\n", moop->l2a->tape_head);
    printf("Total operations: %u\n", moop->l2a->total_ops);
    printf("Tape wrapped: %s\n", moop->l2a->tape_wrapped ? "Yes" : "No");
    printf("Actors: %u\n", moop->l3b->actor_count);
    printf("Protos: %u\n", moop->l3b->proto_count);
    printf("===============================\n");
}
