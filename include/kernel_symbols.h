#pragma once
/** @brief Start address of the kernel image in physical memory. */
extern u8 _begin[];

/** @brief End address of the kernel image in physical memory. */
extern u8 _kernend[];

/** @brief Start of the .text section (kernel code). */
extern u8 _btext[];

/** @brief End of the .text section (kernel code). */
extern u8 _etext[];

/** @brief Start of the .idt section (reserved IDT page). */
extern u8 _bidt[];

/** @brief Address of the IDT base. */
extern u8 _idt[];

/** @brief End of the .idt section. */
extern u8 _eidt[];

/** @brief Start of the .rodata section (read‑only data). */
extern u8 _brodata[];

/** @brief End of the .rodata section. */
extern u8 _erodata[];

/** @brief Start of the .bss section (zero‑initialized data). */
extern u8 _bbss[];

/** @brief End of the .bss section. */
extern u8 _ebss[];

/** @brief Start of the .data section (writable data). */
extern u8 _bdata[];

/** @brief End of the .data section. */
extern u8 _edata[];

/** @brief Kernel size in 512‑byte sectors.
 *  @note Matches the standard disk sector size used by the bootloader.
 */
extern u8 _kernSize[];
