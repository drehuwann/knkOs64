#pragma once

#include "typedefs.h"
#include "mmap.h"

/// @brief computed at boot time, according to MAXPHYADDR
extern const u64 pageFrameMask;

enum PTEflagsLo {
    PTEpresent = 1 << 0,
    PTEwritable = 1 << 1,
    PTEuser = 1 << 2,
    PTEwritethrough = 1 << 3,
    PTEcachedisable = 1 << 4,
    PTEaccessed = 1 << 5,
    PTEdirty = 1 << 6,
    PTEpat = 1 << 7,
    PTEglobal = 1 << 8
};

enum PTEflagsHi {
    PTEprotkey = 1 << 27 | 1 << 28 | 1 << 29 | 1 << 30,
    PTEexecdisable = 1 << 31
};

typedef union {
    struct {
        u32 lo; /**< Low 32-bit flags (bits 0–31 of entry) */
        u32 hi; /**< High 32-bit flags (bits 32–63 of entry) */
    };
    u64 value; /**< Combined 64-bit entry value */
} PTEflags; /* Assumes little-endian layout */

typedef u64 PTE;

/**
 * @brief Commonly used PTE flag presets.
 *
 * Low half uses enum PTEflagsLo, high half uses enum PTEflagsHi.
 * Assumes little-endian layout.
 */
static const PTEflags PTE_RW_X  = { .lo = PTEpresent | PTEwritable, .hi = 0 };
static const PTEflags PTE_RO_X  = { .lo = PTEpresent,               .hi = 0 };
static const PTEflags PTE_RW_NX = { .lo = PTEpresent | PTEwritable, .hi = PTEexecdisable };
static const PTEflags PTE_RO_NX = { .lo = PTEpresent,               .hi = PTEexecdisable };

typedef struct {
    PTE ptEntries[512];
} PT;

enum PDEflagsLo {
    PDEpresent = 1 << 0,
    PDEwritable = 1 << 1,
    PDEuser = 1 << 2,
    PDEwritethrough = 1 << 3,
    PDEcachedisable = 1 << 4,
    PDEaccessed = 1 << 5,
    PDEdirty = 1 << 6,
    PDEsize = 1 << 7,
    PDEglobal = 1 << 8
};

enum PDEflagsHi {
    PDEprotkey = 1 << 27 | 1 << 28 | 1 << 29 | 1 << 30,
    PDEexecdisable = 1 << 31
};

typedef union {
    struct {
        u32 lo; /**< Low 32-bit flags (bits 0–31 of entry) */
        u32 hi; /**< High 32-bit flags (bits 32–63 of entry) */
    };
    u64 value; /**< Combined 64-bit entry value */
} PDEflags; /* Assumes little-endian layout */

typedef u64 PDE;

typedef struct {
    PDE pdEntries[512];
} PD;

enum PDPTEflagsLo {
    PDPTEpresent = 1 << 0,
    PDPTEwritable = 1 << 1,
    PDPTEuser = 1 << 2,
    PDPTEwritethrough = 1 << 3,
    PDPTEcachedisable = 1 << 4,
    PDPTEaccessed = 1 << 5,
    PDPTEdirty = 1 << 6,
    PDPTEsize = 1 << 7,
    PDPTEglobal = 1 << 8
};

enum PDPTEflagsHi {
    PDPTEprotkey = 1 << 27 | 1 << 28 | 1 << 29 | 1 << 30,
    PDPTEexecdisable = 1 << 31
};

typedef union {
    struct {
        u32 lo; /**< Low 32-bit flags (bits 0–31 of entry) */
        u32 hi; /**< High 32-bit flags (bits 32–63 of entry) */
    };
    u64 value; /**< Combined 64-bit entry value */
} PDPTEflags; /* Assumes little-endian layout */

typedef u64 PDPTE;

typedef struct {
    PDPTE pdptEntries[512];
} PDPT;

enum PML4EflagsLo {
    PML4Epresent      = 1 << 0,
    PML4Ewritable     = 1 << 1,
    PML4Euser         = 1 << 2,
    PML4Ewritethrough = 1 << 3,
    PML4Ecachedisable = 1 << 4,
    PML4Eaccessed     = 1 << 5
    // bits 6–8 reserved
};

enum PML4EflagsHi {
    PML4Eprotkey     = (1 << 27) | (1 << 28) | (1 << 29) | (1 << 30),
    PML4Eexecdisable = 1 << 31
};

typedef union {
    struct {
        u32 lo; /**< Low 32-bit flags (bits 0–31 of entry) */
        u32 hi; /**< High 32-bit flags (bits 32–63 of entry) */
    };
    u64 value; /**< Combined 64-bit entry value */
} PML4Eflags; /* Assumes little-endian layout */

typedef u64 PML4E;

typedef struct {
    PML4E PML4Entries[512];
} PML4;

/// @brief requires a page from physical memory manager. Maps it as entry
/// @param entry address of PTE to map to
/// @return false on error
bool vm_alloc_page(PTE *entry);

/// @brief calls physical free() and marks this entry not-present
/// @param entry address of PTE
void vm_free_page(PTE *entry);

/// @brief finds and returns address of entry which maps addr in table p 
/// @param p p pointer on page table 
/// @param addr virtual address to find
/// @return pointer on entry in table
PTE *PT_lookup(PT *p, void *addr);

/// @brief finds and returns address of entry which maps addr in PD p 
/// @param p p pointer on page directory table
/// @param addr virtual address to find
/// @return pointer on entry in page directory
PDE *PD_lookup(PD *p, void *addr);

/// @brief finds and returns address of entry which maps addr in PDPT p 
/// @param p p pointer on page directory pointer table
/// @param addr virtual address to find
/// @return pointer on entry in page directory pointer table
PDPTE *PDPT_lookup(PDPT *p, void *addr);

/// @brief finds and returns address of entry which maps addr in PML4 p 
/// @param p p pointer on PML4 table
/// @param addr virtual address to find
/// @return pointer on entry in PML4 table
PML4E *PML4_lookup(PML4 *p, void *addr);

/// @brief updates cr3 with pml4
/// @param pml4 pointer on pml4 to switch to 
/// @return false on failure
bool vm_switch_pml4(PML4 *pml4);

/// @brief 
/// @return pointer on current PML4
PML4 *vm_getPML4();

/// @brief clears translation lookaside buffer entry specified by addr
/// @param addr virtual address
void vm_flush_tlb_entry(void *addr);

/**
 * @brief Map a single 4 KiB page with specific PTE flags.
 *
 * Uses ensure_* helpers to create intermediate paging structures as needed.
 * Sets the final PTE to point to the given physical frame with the requested attributes.
 *
 * @param phys   Physical address of the 4 KiB frame (must be aligned).
 * @param virt   Virtual address to map (must be aligned).
 * @param flags  Combined low/high PTE flags.
 *
 * @note Writable is propagated up the hierarchy if requested.
 *       Execute-disable is applied via PTEexecdisable in the high flags.
 */
void vm_map_page_flags(void *phys, void *virt, PTEflags pte_flags);

/**
 * @brief Map a single 4 KiB page as present and writable (executable).
 *
 * Backward-compatible wrapper for vm_map_page_flags() that preserves
 * the original vm_map_page() behavior.
 *
 * @param phys  Physical address of the 4 KiB frame (must be aligned).
 * @param virt  Virtual address to map (must be aligned).
 */
void vm_map_page(void *phys, void *virt);

/**
 * @brief Sets up the kernel's full virtual memory layout after entering long mode.
 *
 * This function is called once the kernel has been loaded and control has
 * transferred from the bootloader. At this point, the CPU is already in
 * long mode with a minimal identity mapping established by early boot code
 * (e.g., setupIdentityPaging in assembly) to enable the transition.
 *
 * vm_initialize() replaces that temporary mapping with the kernel's complete
 * paging structures. It allocates and clears the top-level PML4 and lower
 * tables, establishes identity mappings for any low physical memory still
 * needed, and maps the kernel's higher-half virtual address space to its
 * physical location.
 *
 * @note Requires the physical memory manager (PMM) to be initialized, as it
 *       allocates page-aligned blocks for page tables and pages.
 * @note Should be called early in kernel initialization, before performing
 *       any dynamic virtual memory mappings or switching to user mode.
 */
void vm_initialize();
