#!/bin/bash

mkdir -pv build
mkdir -pv output

pushd build
    cp ../link.ld ./
    nasm ../src/sector1/bootloader.asm -f bin -i ../src/sector1 -o bootloader.bin
    nasm ../src/sector2+/extended.asm -f elf64 -i ../src/sector2+ -o extended.o
    nasm ../src/sector2+/binaries.asm -f elf64 -i ../src/sector2+ -o binaries.o
    cmake ../CMakeLists.txt -G "Unix Makefiles" -D CMAKE_C_COMPILER=x86_64-elf-gcc
    make -f "Makefile"
    dd if=/dev/zero of=../output/knkOs64.img count=1008 bs=512
    dd if=bootloader.bin of=../output/knkOs64.img conv=notrunc
    dd if=kern.bin of=../output/knkOs64.img bs=512 seek=1 conv=notrunc
popd

###########
#   config environment nefore run
###########
##sed patterns
mark="-qf .bochsrc"
switchOnOff="-rc debug.rc"
file2feedSed="./.vscode/tasks.json" 
target2link2=".bochsrc"
# 
#####################################33
#   check if bochs is available
BOCHS=$(which bochs)
if [[ -n $BOCHS ]]; then
    # Frees the old symbolic sync `.bochsrc`
    rm .bochsrc
    # Check if $BOCHS was built with gdbstub    
    if [[ $(readelf -p .dynstr $BOCHS | grep gdbstub | wc -l) -ne 0 ]]; then
        # Remove the switch from tasks.json
        sed -i "s/$switch[[:space:]]*//g" "$tasksJson"
        # Hook the symbolic link to the GDB_specific `.bochsrc`
        ln -s .bochsrcGDB_noSMP $target2link2
    else
        # gdbstub is not present : Add the switch into tasks.sjon
        sed -i "/$pattern/!{h;d}; /$text/!{s/$pattern/$pattern $text/;}" "$file2feedSed"
        # Hook the symbolic link to the SMP_specific `.bochsrc`
        ln -s .bochsrcSMP_noGDB $target2link2
    fi
fi
