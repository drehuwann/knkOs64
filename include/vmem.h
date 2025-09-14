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

typedef u64 PTE;

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

typedef u64 PDPTE;

typedef struct {
    PDPTE pdptEntries[512];
} PDPT;

enum PML4Eflags {
    PML4Epresent = 1 << 0,
    PML4Ewritable = 1 << 1,
    PML4Euser = 1 << 2,
    PML4Ewritethrough = 1 << 3,
    PML4Ecachedisable = 1 << 4,
    PML4Eaccessed = 1 << 5
};

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
PDE *PD_lookup(PD *p, void *addr);

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

/// @brief maps phys to virt. Allocates what is needed to
/// @param phys physical address
/// @param virt virtual address
void vm_map_page(void *phys, void *virt);
