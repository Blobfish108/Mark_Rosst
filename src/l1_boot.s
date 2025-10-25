// l1_boot.s
// L1: System-Based Processing (M1 Assembly Bootloader)
// Apple Silicon ARM64 assembly

.global _l1_boot
.global _l1_init_memory
.align 4

// ============================================================================
// L1 Boot: Initialize hardware and jump to L2a
// ============================================================================

_l1_boot:
    // Save link register
    stp     x29, x30, [sp, #-16]!
    mov     x29, sp

    // Initialize memory subsystem
    bl      _l1_init_memory

    // Initialize qubit array (passed as argument in x0 = qubit_count)
    // x0 = qubit count
    // Returns pointer to qubit array in x0
    bl      _l1_alloc_qubits

    // Initialize history array
    // x0 = history capacity (4096 cells)
    mov     x0, #4096
    bl      _l1_alloc_history

    // Restore and return to C runtime
    ldp     x29, x30, [sp], #16
    ret

// ============================================================================
// L1 Memory Initialization
// ============================================================================

_l1_init_memory:
    stp     x29, x30, [sp, #-16]!
    mov     x29, sp

    // Set up page tables (minimal - identity map first 1GB)
    // This is simplified - production would set up proper MMU

    // Clear BSS section
    adrp    x0, __bss_start
    add     x0, x0, :lo12:__bss_start
    adrp    x1, __bss_end
    add     x1, x1, :lo12:__bss_end

clear_bss_loop:
    cmp     x0, x1
    b.ge    clear_bss_done
    str     xzr, [x0], #8
    b       clear_bss_loop

clear_bss_done:
    ldp     x29, x30, [sp], #16
    ret

// ============================================================================
// L1 Qubit Array Allocation (Cache-Aligned)
// ============================================================================

_l1_alloc_qubits:
    stp     x29, x30, [sp, #-16]!
    mov     x29, sp

    // x0 = qubit count
    // Allocate cache-aligned memory (64-byte alignment for M1)

    // Round up to cache line size
    add     x0, x0, #63
    and     x0, x0, #-64

    // Call malloc (assumes C runtime available)
    bl      _malloc

    // Zero the memory
    mov     x1, x0          // Save pointer
    mov     x2, #0          // Byte count (from original x0)
zero_qubits:
    cbz     x2, zero_done
    strb    wzr, [x1], #1
    sub     x2, x2, #1
    b       zero_qubits

zero_done:
    ldp     x29, x30, [sp], #16
    ret

// ============================================================================
// L1 History Array Allocation
// ============================================================================

_l1_alloc_history:
    stp     x29, x30, [sp, #-16]!
    mov     x29, sp

    // x0 = capacity (number of R_Cell structs)
    // R_Cell is 4 bytes, so multiply by 4
    lsl     x0, x0, #2

    // Allocate
    bl      _malloc

    ldp     x29, x30, [sp], #16
    ret

// ============================================================================
// L1 Fast Qubit Access (Inline Assembly Candidates)
// ============================================================================

.global _l1_read_qubit_fast
_l1_read_qubit_fast:
    // x0 = qubit_array pointer
    // x1 = index
    // Returns: x0 = qubit value (0 or 1)

    ldrb    w0, [x0, x1]
    ret

.global _l1_write_qubit_fast
_l1_write_qubit_fast:
    // x0 = qubit_array pointer
    // x1 = index
    // x2 = value (0 or 1)

    and     w2, w2, #1      // Mask to single bit
    strb    w2, [x0, x1]
    ret

.global _l1_xor_qubit_fast
_l1_xor_qubit_fast:
    // x0 = qubit_array pointer
    // x1 = index
    // XOR qubit at index (for NOT operation)

    ldrb    w2, [x0, x1]
    eor     w2, w2, #1
    strb    w2, [x0, x1]
    ret

// ============================================================================
// L1 BSS Section Markers (for clearing)
// ============================================================================

.section __DATA,__bss
.align 3
__bss_start:
    .space 0

.section __DATA,__bss_end
.align 3
__bss_end:
    .space 0
