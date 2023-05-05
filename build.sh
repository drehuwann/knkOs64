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
    cat bootloader.bin kern.bin > ../output/bootloader.flp
popd
