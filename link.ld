OUTPUT_FORMAT(binary)

ENTRY(_start)

INPUT
(
    extended.o
    binaries.o
    libkernel.a
)

OUTPUT
(
    kern.bin
)

SECTIONS
{
    . = 0x8000;
    HIDDEN(_begin = .);
    .text : ALIGN(0x1000)
    {
    _btext = .;
        *(.text)
    _etext = .;
    }

    .idt BLOCK(0x1000) : ALIGN(0x1000)
    {
    _bidt = .;
        _idt = .;
        . = . + 0x1000;
    _eidt = .;
    }

    .rodata : ALIGN(0x1000)
    {
    _brodata = .;
        *(.rodata)
    _erodata = .;
    }

/* don't put .bss as the last section, as LD will optimize its size, and we will
need an ELF-loader to re-expand the .bss section to its running size ... */
    .bss : ALIGN(0x1000)
    {
    _bbss = .;
        *(COMMON)
        *(.bss)
    _ebss = .;
    }

    .data : ALIGN(0x1000)
    {
    _bdata = .;
        *(.data)
    . = ALIGN(0x1000); /* finally ALIGN to the next 'page-size' chunk */
    _edata = .;
    }

    _kernSize = (. - _begin) >> 9; /*computes kernel size in 512bytes sectors*/
}
