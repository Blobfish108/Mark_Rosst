// 03_self_modification.c
// Demonstrates self-modification (homoiconicity)

#include "../src/moop_enhanced.h"
#include <stdio.h>

int main() {
    printf("=== Moop Example 3: Self-Modification ===\n\n");

    Moop_Runtime* moop = moop_init(8, 3);
    if (!moop) return 1;

    // Execute some operations
    printf("Executing initial operations...\n");
    l2a_NOT(moop->l2a, 0);
    l2a_CNOT(moop->l2a, 0, 1);
    l2a_SWAP(moop->l2a, 1, 2);

    // Read from tape (code as data)
    printf("\nReading tape (code as data):\n");
    for (uint32_t i = 0; i < 3; i++) {
        R_Cell cell = l2a_read_tape(moop->l2a, i);
        const char* gates[] = {"CCNOT", "CNOT", "NOT", "SWAP"};
        printf("  Tape[%u]: %s(%u, %u, %u)\n",
               i, gates[cell.gate], cell.a, cell.b, cell.c);
    }

    // Modify the tape (data as code)
    printf("\nModifying tape entry[1]: CNOT -> NOT...\n");
    R_Cell modified = {2, 5, 0, 0};  // NOT(5)
    l2a_write_tape(moop->l2a, 1, modified);

    // Read back modified tape
    R_Cell readback = l2a_read_tape(moop->l2a, 1);
    const char* gates[] = {"CCNOT", "CNOT", "NOT", "SWAP"};
    printf("  Tape[1] after modification: %s(%u, %u, %u)\n",
           gates[readback.gate], readback.a, readback.b, readback.c);

    // Meta-modification: apply a rule to the tape itself
    printf("\nMeta-modifying: applying transformation rule...\n");
    R_Cell rule[] = {
        {2, 6, 0, 0},  // NOT(6)
        {2, 7, 0, 0}   // NOT(7)
    };
    l2a_meta_modify(moop->l2a, rule, 2);

    printf("  Meta-modification applied (2 operations added)\n");
    printf("  Total operations: %u\n", moop->l2a->total_ops);

    moop_free(moop);
    printf("\n✓ Example complete\n");
    return 0;
}
