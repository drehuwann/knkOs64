#include "vmem.h"

#define PML4_INDEX(x) ((((u64)(x)) >> 39) & 0x1ff)
#define PDPT_INDEX(x) ((((u64)(x)) >> 30) & 0x1ff)
#define PD_INDEX(x) ((((u64)(x)) >> 21) & 0x1ff)
#define PT_INDEX(x) ((((u64)(x)) >> 12) & 0x1ff)
#define GET_PHYS_ADDR(x) (*x & ~0xfff)

/// current directory table (global)
static PML4 *current_PML4 = (PML4 *)0x1000;

static u64 mkMask(u32 hi, u32 lo) {
    u64 toRet = hi;
    toRet << 32;
    toRet += lo;
}

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

static inline void PDE_enable_global(PDE entry) { //do nothing for now
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

static inline void PDPTE_enable_global(PDPTE entry) { //do nothing for now
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

void vm_map_page(void *phys, void *virt) {
    PML4 *pml4 = vm_getPML4();
    PML4E *pml4e = &pml4->PML4Entries[PML4_INDEX(virt)];
    if (!(* pml4e)) {
        PDPT *pdpt = (PDPT *)pm_alloc_block();
        if (!pdpt) return;
        memset(pdpt, 0, sizeof(PDPT));
        PML4E_set_frame(pml4e, (void *)pdpt);
    }
    PML4E_add_attrib(pml4e, PML4Epresent | PML4Ewritable);
    PML4E_set_frame(pml4e, (void *)(*pml4e));

    PDPT *pdpt = (PDPT *)PML4E_pfn(* pml4e);
    PDPTE *pdpte = &pdpt->pdptEntries[PDPT_INDEX(virt)];
    if (!(* pdpte)) {
        PD *pd = (PD *)pm_alloc_block();
        if (!pd) return;
        memset(pd, 0, sizeof(PDPT));
        PDPTE_set_frame(pdpte, (void *)pd);
    }
    PDPTE_add_attrib(pdpte, PDPTEpresent | PDPTEwritable);
    PDPTE_set_frame(pdpte, (void *)(*pdpte));

    PD *pd = (PD *)PDPTE_pfn(* pdpte);
    PDE *pde = &pd->pdEntries[PD_INDEX(virt)];
    if (!(* pde)) {
        PT *pt = (PT *)pm_alloc_block();
        if (!pt) return;
        memset(pt, 0, sizeof(PDPT));
        PDE_set_frame(pde, (void *)pt);
    }
    PDE_add_attrib(pde, PDEpresent | PDEwritable);
    PDE_set_frame(pde, (void *)(*pde));

    PT *pt = (PT *)PDE_pfn(* pde);
    PTE *pte = &pt->ptEntries[PT_INDEX(virt)];
    if (!(*pte)) {
        PTE *page = (PTE *)pm_alloc_block();
        if (!page) return;
        memset (page, 0, 0x1000);
        PTE_set_frame(pte, (void *)page);
    }
    PTE_add_attrib(pte, PTEpresent | PTEwritable);
    PTE_set_frame(pte, phys);
}

void vm_initialize() {
     
    	//! allocate default page table
    	PT* table = (PT *)pm_alloc_block ();
    	if (!table) return;
     
    	//! allocates 3gb page table
    	PT* table2 = (PT*) pm_alloc_block ();
    	if (!table2) return;

    	//! clear page table
    	vm_ptable_clear (table);

//Remember how page tables must be located at 4K aligned addresses? Thanks to
//out physical memory manager (PMM), our pmmngr_alloc_block() already does just
//this so we do not need to worry about it. Because a single block allocated is
//already 4K in size, the page table has enough storage space for its entries as
//well
//(1024 page table entries * 4 bytes per entry (size of page table entry) = 4K)
//so all we need is a single block.

//Afterwords we clear out the page table to clean it up for our use.

    //! 1st 4mb are idenitity mapped
    for (int i=0, frame=0x0, virt=0x00000000; i<0x200; i++, frame+=0x1000,
                virt+=0x1000) {
     		//! create a new page
    	PTE page = 0;
    	PTE_add_attrib(&page, PTEpresent);
        PTE_set_frame (&page, frame);

    		//! ...and add it to the page table
    	table2->ptEntries[PT_INDEX(virt)] = page;
    }

//This parts a little tricky. Remember that as soon as paging is enabled, all
//address become virtual? This poses a problem. To fix this, we must map the
//virtual addresses to the same physical addresses so they refer to the same
//thing. This is idenitity mapping.

//The above code idenitity maps the page table to the first 4MB of physical
//memory (the entire page table). It creates a new page and sets its PRESENT bit
//followed by the frame address we want the page to refer to. Afterwords it
//converts the current virtual address we are mapping (stored in "frame") to a
//page table index to set that page table entry.

//We increment "frame" for each page in the page table (stored in "i") by 4K
//(4096) as that is the block of memory each page refrences. (Remember page
//table index 0 references address 0 - 4093, index 1 refrences address
//4096--etc..?)

//Here we run into a problem. Because the boot loader maps and loads the kernel
//directly to 3gb virtual, we also need to remap the area where the kernel is
//at:

    	//! map 1mb to 3gb (where we are at)
    	for (int i=0, frame=0x100000, virt=0xc0000000; i<1024; i++,
                frame+=4096, virt+=4096) {

    		//! create a new page
    		PTE page=0;
    		PTE_add_attrib (&page, PTEpresent);
    		PTE_set_frame (&page, frame);

    		//! ...and add it to the page table
    		table->ptEntries[PAGE_TABLE_INDEX(virt)] = page;
    	}
}
