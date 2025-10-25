// 04_trinary_maybe.c
// Demonstrates trinary MAYBE with LLM confidence

#include "../src/moop_enhanced.h"
#include <stdio.h>

int main() {
    printf("=== Moop Example 4: Trinary MAYBE ===\n\n");

    // Create MAYBE states with different resolutions
    printf("Creating MAYBE states...\n\n");

    // 1. Unresolved state
    L2b_Maybe auth = l2b_maybe_create("user_authenticated");
    printf("1. Created MAYBE: '%s'\n", auth.condition_name);
    printf("   State: %s\n",
           auth.state == MAYBE_UNRESOLVED ? "UNRESOLVED" :
           auth.state == MAYBE_TRUE ? "TRUE" : "FALSE");

    // 2. Resolve to TRUE with high confidence
    printf("\n2. Resolving to TRUE with confidence 0.95...\n");
    l2b_maybe_resolve(&auth, true, 0.95f, "JWT token valid, session active");
    printf("   State: %s\n", auth.state == MAYBE_TRUE ? "TRUE" : "FALSE");
    printf("   Confidence: %.2f\n", auth.confidence);
    printf("   Reasoning: %s\n", auth.llm_reasoning);
    printf("   Is resolved? %s\n", l2b_maybe_is_resolved(&auth) ? "Yes" : "No");

    // 3. Resolve to FALSE with lower confidence
    L2b_Maybe permission = l2b_maybe_create("has_admin_permission");
    printf("\n3. Created MAYBE: '%s'\n", permission.condition_name);
    l2b_maybe_resolve(&permission, false, 0.73f, "User role is 'guest', not admin");
    printf("   State: %s\n", permission.state == MAYBE_FALSE ? "FALSE" : "TRUE");
    printf("   Confidence: %.2f\n", permission.confidence);
    printf("   Reasoning: %s\n", permission.llm_reasoning);

    // 4. Demonstrate unresolved state handling
    L2b_Maybe unknown = l2b_maybe_create("network_reachable");
    printf("\n4. Unresolved MAYBE: '%s'\n", unknown.condition_name);
    printf("   State: %s\n", unknown.state == MAYBE_UNRESOLVED ? "UNRESOLVED" : "resolved");
    printf("   Can proceed? %s\n",
           l2b_maybe_is_resolved(&unknown) ? "Yes" : "No - must wait for resolution");

    // Cleanup
    l2b_maybe_free(&auth);
    l2b_maybe_free(&permission);
    l2b_maybe_free(&unknown);

    printf("\n✓ Example complete\n");
    return 0;
}
