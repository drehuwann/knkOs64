#include "vmem.h"
#include "kernel_symbols.h"
#include "memory.h"
#include "typedefs.h"

enum {
    PAGE_SIZE        = 0x1000,
    KERNEL_VIRT_BASE = 0xFFFFFFFF80000000ULL
};

enum {
    PTE_PRESENT = 1 << 0,
    PTE_WRITABLE = 1 << 1,
    PTE_NO_EXEC  = 1ULL << 63 // NX if supported
};

#define PML4_INDEX(x) ((((u64)(x)) >> 39) & 0x1ff)
#define PDPT_INDEX(x) ((((u64)(x)) >> 30) & 0x1ff)
#define PD_INDEX(x) ((((u64)(x)) >> 21) & 0x1ff)
#define PT_INDEX(x) ((((u64)(x)) >> 12) & 0x1ff)
#define GET_PHYS_ADDR(x) (*x & ~0xfff)

/// current directory table (global)
static PML4 *current_PML4 = (PML4 *)0x1000;

static inline void PTE_add_attrib(PTE *entry, u64 attrib) {
    if (entry != (PTE *)0) {
        *entry |= attrib;
    }
}

static inline void PTE_del_attrib(PTE *entry, u64 attrib) {
    if (entry != (PTE *)0) {
        *entry &= ~attrib;
    }
}

static inline void PTE_set_frame(PTE *entry, void *frame) {
    if (entry != (PTE *)0) {
        *entry &= ~pageFrameMask; // first clear frame field
        *entry |= ((u64)frame & pageFrameMask); //write frame in entry
    }
}

static inline bool PTE_is_present(PTE entry) {
    return ((entry & (u64)PTEpresent) != 0);
}

static inline bool PTE_is_writable(PTE entry) {
    return ((entry & (u64)PTEwritable) != 0);
}

static inline void *PTE_pfn(PTE entry) {
    return ((void *)(entry & pageFrameMask));
}

static inline void PDE_add_attrib(PDE *entry, u64 attrib) {
    if (entry != (PDE *)0) {
        *entry |= attrib;
    }
}

static inline void PDE_del_attrib(PDE *entry, u64 attrib) {
    if (entry != (PDE *)0) {
        *entry &= ~attrib;
    }
}

static inline void PDE_set_frame(PDE *entry, void *frame) {
    if (entry != (PDE *)0) {
        *entry &= ~pageFrameMask; // first clear frame field
        *entry |= ((u64)frame & pageFrameMask); //write frame in entry
    }
}

static inline bool PDE_is_present(PDE entry) {
    return ((entry & (u64)PDEpresent) != 0);
}

static inline bool PDE_is_user(PDE entry) {
    return ((entry & (u64)PDEuser) != 0);
}

static inline bool PDE_is_writable(PDE entry) {
    return ((entry & (u64)PDEwritable) != 0);
}

static inline bool PDE_is_2mb(PDE entry) {
    return ((entry & (u64)PDEsize) != 0);
}

static inline void *PDE_pfn(PDE entry) {
    return ((void *)(entry & pageFrameMask));
}

static inline void PDE_enable_global(PDE /*entry*/) {
    //do nothing for now
}

static inline void PDPTE_add_attrib(PDPTE *entry, u64 attrib) {
    if (entry != (PDPTE *)0) {
        *entry |= attrib;
    }
}

static inline void PDPTE_del_attrib(PDPTE *entry, u64 attrib) {
    if (entry != (PDPTE *)0) {
        *entry &= ~attrib;
    }
}

static inline void PDPTE_set_frame(PDPTE *entry, void *frame) {
    if (entry != (PDPTE *)0) {
        *entry &= ~pageFrameMask; // first clear frame field
        *entry |= ((u64)frame & pageFrameMask); //write frame in entry
    }
}

static inline bool PDPTE_is_present(PDPTE entry) {
    return ((entry & (u64)PDPTEpresent) != 0);
}

static inline bool PDPTE_is_user(PDPTE entry) {
    return ((entry & (u64)PDPTEuser) != 0);
}

static inline bool PDPTE_is_writable(PDPTE entry) {
    return ((entry & (u64)PDPTEwritable) != 0);
}

static inline bool PDPTE_is_1gb(PDPTE entry) {
    return ((entry & (u64)PDPTEsize) != 0);
}

static inline void *PDPTE_pfn(PDPTE entry) {
    return ((void *)(entry & pageFrameMask));
}

static inline void PDPTE_enable_global(PDPTE /*entry*/) {
    //do nothing for now
}

static inline void PML4E_add_attrib(PML4E *entry, u64 attrib) {
    if (entry != (PML4E *)0) {
        *entry |= attrib;
    }
}

static inline void PML4E_del_attrib(PML4E *entry, u64 attrib) {
    if (entry != (PML4E *)0) {
        *entry &= ~attrib;
    }
}

static inline void PML4E_set_frame(PML4E *entry, void *frame) {
    if (entry != (PML4E *)0) {
        *entry &= ~pageFrameMask; // first clear frame field
        *entry |= ((u64)frame & pageFrameMask); //write frame in entry
    }
}

static inline bool PML4E_is_present(PML4E entry) {
    return ((entry & (u64)PML4Epresent) != 0);
}

static inline bool PML4E_is_user(PML4E entry) {
    return ((entry & (u64)PML4Euser) != 0);
}

static inline bool PML4E_is_writable(PML4E entry) {
    return ((entry & (u64)PML4Ewritable) != 0);
}

static inline void *PML4E_pfn(PML4E entry) {
    return ((void *)(entry & pageFrameMask));
}

bool vm_alloc_page(PTE *entry) {
	void* p = pm_alloc_block();
	if (!p) return false;
	PTE_set_frame(entry, p);
	PTE_add_attrib(entry, PTEpresent);
	return true;
}

void vm_free_page (PTE *entry) {
    void* p = PTE_pfn(*entry);
    if (p) pm_free_block(p);
    PTE_del_attrib(entry, PTEpresent);
}

inline PTE *PT_lookup(PT *p, void *addr) {
	return (p? &p->ptEntries[PT_INDEX(addr)] : (PTE *)0);
}

inline PDE *PD_lookup(PD* p, void *addr) {
    return (p? &p->pdEntries[PT_INDEX(addr)] : (PDE *)0);
}

inline PDPTE *PDPT_lookup(PDPT* p, void *addr) {
    return (p? &p->pdptEntries[PT_INDEX(addr)] : (PDPTE *)0);
}

inline PML4E *PML4_lookup(PML4* p, void *addr) {
    return (p? &p->PML4Entries[PT_INDEX(addr)] : (PDE *)0);
}
 
inline bool vm_switch_pml4(PML4* pml4) {
    if (!pml4) return false;
    current_PML4 = pml4;
    __asm__ volatile("movq %0, %%rax\nmovq %%rax, %%cr3"::"m"(pml4));
	return true;
}

PML4 *vm_getPML4() {
    return current_PML4;
}

void vm_flush_tlb_entry(void *addr) {
    __asm__ volatile("cli\ninvlpg %0\nsti"::"m"(addr));
}

/**
 * @brief Ensure a PML4 entry exists for a given virtual address.
 *
 * Allocates and zeroes a new PDPT if the PML4 entry is empty.
 *
 * @param virt   Virtual address to ensure mapping for.
 * @param out_e  Output pointer to the PML4 entry.
 */
static inline void ensure_pml4e(void *virt, PML4E **out_e) {
    PML4 *pml4 = vm_getPML4();
    PML4E *e = &pml4->PML4Entries[PML4_INDEX(virt)];
    if (!(*e)) {
        PDPT *pdpt = (PDPT *)pm_alloc_block();
        if (!pdpt) { *out_e = 0; return; }
        memset(pdpt, 0, sizeof(PDPT));
        PML4E_set_frame(e, (void *)pdpt);
    }
    PML4E_add_attrib(e, PML4Epresent);
    *out_e = e;
}

/**
 * @brief Ensure a PDPT entry exists for a given virtual address.
 *
 * Allocates and zeroes a new PD if the PDPTE is empty.
 *
 * @param pml4e  Pointer to the parent PML4 entry.
 * @param virt   Virtual address to ensure mapping for.
 * @param out_e  Output pointer to the PDPTE.
 */
static inline void ensure_pdpte(PML4E *pml4e, void *virt, PDPTE **out_e) {
    PDPT *pdpt = (PDPT *)PML4E_pfn(*pml4e);
    PDPTE *e = &pdpt->pdptEntries[PDPT_INDEX(virt)];
    if (!(*e)) {
        PD *pd = (PD *)pm_alloc_block();
        if (!pd) { *out_e = 0; return; }
        memset(pd, 0, sizeof(PD));
        PDPTE_set_frame(e, (void *)pd);
    }
    PDPTE_add_attrib(e, PDPTEpresent);
    *out_e = e;
}
/**
 * @brief Ensure a PD entry exists for a given virtual address.
 *
 * Allocates and zeroes a new PT if the PDE is empty.
 *
 * @param pdpte  Pointer to the parent PDPTE.
 * @param virt   Virtual address to ensure mapping for.
 * @param out_e  Output pointer to the PDE.
 */
static inline void ensure_pde(PDPTE *pdpte, void *virt, PDE **out_e) {
    PD *pd = (PD *)PDPTE_pfn(*pdpte);
    PDE *e = &pd->pdEntries[PD_INDEX(virt)];
    if (!(*e)) {
        PT *pt = (PT *)pm_alloc_block();
        if (!pt) { *out_e = 0; return; }
        memset(pt, 0, sizeof(PT));
        PDE_set_frame(e, (void *)pt);
    }
    PDE_add_attrib(e, PDEpresent);
    *out_e = e;
}

/**
 * @brief Ensure a PTE exists for a given virtual address.
 *
 * Allocates and zeroes a new physical page if the PTE is empty.
 *
 * @param pde    Pointer to the parent PDE.
 * @param virt   Virtual address to ensure mapping for.
 * @param out_e  Output pointer to the PTE.
 */
static inline void ensure_pte(PDE *pde, void *virt, PTE **out_e) {
    PT *pt = (PT *)PDE_pfn(*pde);
    PTE *e = &pt->ptEntries[PT_INDEX(virt)];
    if (!(*e)) {
        // Allocate a physical page for this PTE to map
        // (In many kernels, you don't allocate here unless you want to
        //  pre-fill with a backing frame; otherwise you just set the frame later)
        // For now, just zero the PTE entry itself
        *e = 0;
    }
    *out_e = e;
}

/**
 * @brief Get the PTE for a given virtual address.
 *
 * @param pde   Pointer to the parent PDE.
 * @param virt  Virtual address to retrieve the PTE for.
 * @return Pointer to the PTE entry.
 */
static inline PTE *get_pte(PDE *pde, void *virt) {
    PT *pt = (PT *)PDE_pfn(*pde);
    return &pt->ptEntries[PT_INDEX(virt)];
}

void vm_map_page_flags(void *phys, void *virt, PTEflags flags) {
    PML4E *pml4e;
    ensure_pml4e(virt, &pml4e);
    if (!pml4e) return;
    if (flags.lo & PTEwritable)
        PML4E_add_attrib(pml4e, PML4Ewritable);

    PDPTE *pdpte;
    ensure_pdpte(pml4e, virt, &pdpte);
    if (!pdpte) return;
    if (flags.lo & PTEwritable)
        PDPTE_add_attrib(pdpte, PDPTEwritable);

    PDE *pde;
    ensure_pde(pdpte, virt, &pde);
    if (!pde) return;
    if (flags.lo & PTEwritable)
        PDE_add_attrib(pde, PDEwritable);

    // Final PTE
    PT *pt = (PT *)PDE_pfn(*pde);
    PTE *pte = &pt->ptEntries[PT_INDEX(virt)];
    PTE_set_frame(pte, phys);
    // Apply low flags directly
    PTE_add_attrib(pte, (u64)flags.lo);
    // Apply high flags by shifting into bits 32–63
    if (flags.hi)
        PTE_add_attrib(pte, ((u64)flags.hi) << 32);
}

/**
 * @brief Map a single 4 KiB page as present and writable (executable).
 *
 * Backward-compatible wrapper for vm_map_page_flags() that preserves
 * the original vm_map_page() behaviour.
 */
void vm_map_page(void *phys, void *virt) {
    PTEflags def = { .lo = PTEpresent | PTEwritable, .hi = 0 };
    vm_map_page_flags(phys, virt, def);
}

/**
 * @brief Map a physical range to the same virtual addresses (identity mapping).
 *
 * @param phys_start  Physical start address.
 * @param phys_end    Physical end address (exclusive).
 * @param flags       Combined low/high PTE flags.
 */
static void map_range_identity(u64 phys_start, u64 phys_end, PTEflags flags) {
    u64 p   = phys_start & ~(PAGE_SIZE - 1);
    u64 end = (phys_end + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    for (; p < end; p += PAGE_SIZE) {
        vm_map_page_flags((void *)p, (void *)p, flags);
    }
}

/**
 * @brief Map a physical range to a given virtual start address.
 *
 * @param phys_start  Physical start address.
 * @param virt_start  Virtual start address.
 * @param size        Size of the range in bytes.
 * @param flags       Combined low/high PTE flags.
 */
static void map_range_at(u64 phys_start, u64 virt_start, u64 size, PTEflags flags) {
    u64 p   = phys_start & ~(PAGE_SIZE - 1);
    u64 v   = virt_start & ~(PAGE_SIZE - 1);
    u64 end = (phys_start + size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    for (; p < end; p += PAGE_SIZE, v += PAGE_SIZE) {
        vm_map_page_flags((void *)p, (void *)v, flags);
    }
}

void vm_initialize(void) {
    // 1. Allocate and clear a new PML4 for the kernel's final page tables
    PML4 *new_pml4 = (PML4 *)pm_alloc_block();
    if (!new_pml4) return;
    memset(new_pml4, 0, sizeof(PML4));

    // Switch CR3 to the new PML4 so vm_map_page_flags() populates it
    vm_switch_pml4(new_pml4);

    // 2. Identity-map low physical memory up to the end of the kernel image
    //    This keeps BIOS data, boot structures, and the kernel's load region accessible.
    map_range_identity(0x0000, (u64)_kernend, PTE_RW_NX);

    // Get the kernel's physical load base from the linker symbol _begin
    u64 phys_base = (u64)_begin;

    // 3. Map kernel sections into higher-half virtual addresses with proper flags

    // .text: present, read-only, executable
    map_range_at((u64)_btext,
                 KERNEL_VIRT_BASE + ((u64)_btext - phys_base),
                 (u64)_etext - (u64)_btext,
                 PTE_RO_X);

    // .rodata: present, read-only, non-executable
    map_range_at((u64)_brodata,
                 KERNEL_VIRT_BASE + ((u64)_brodata - phys_base),
                 (u64)_erodata - (u64)_brodata,
                 PTE_RO_NX);

    // .data: present, writable, non-executable
    map_range_at((u64)_bdata,
                 KERNEL_VIRT_BASE + ((u64)_bdata - phys_base),
                 (u64)_edata - (u64)_bdata,
                 PTE_RW_NX);

    // .bss: present, writable, non-executable
    map_range_at((u64)_bbss,
                 KERNEL_VIRT_BASE + ((u64)_bbss - phys_base),
                 (u64)_ebss - (u64)_bbss,
                 PTE_RW_NX);

    // .idt: present, read-only, non-executable (change to writable if you modify it later)
    map_range_at((u64)_bidt,
                 KERNEL_VIRT_BASE + ((u64)_bidt - phys_base),
                 (u64)_eidt - (u64)_bidt,
                 PTE_RO_NX);

    // 4. Map other runtime structures (e.g., PMBM, framebuffer) here if needed
    // map_range_at(PMBM_phys, PMBM_virt, PMBM_size, PTE_RW_NX);

    // At this point, the new PML4 is active and the TLB has been flushed by CR3 load.
}
