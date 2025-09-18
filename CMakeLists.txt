cmake_minimum_required(VERSION 3.31)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project("knkOs")

include_directories("${PROJECT_SOURCE_DIR}/src")

set (CMAKE_C_FLAGS "-Ttext 0x8000 -I${PROJECT_SOURCE_DIR}/include -m64 -nolibc \
                        -nostdinc -ffreestanding -mno-red-zone")
set (CMAKE_EXE_LINKER_FLAGS "-T\"link.ld\"")

FILE (GLOB_RECURSE kernel_C_SOURCES "*.c")

set_source_files_properties(src/interrupts.c PROPERTIES COMPILE_FLAGS "\
                                                        -mgeneral-regs-only")

ADD_LIBRARY (kernel STATIC
                ${kernel_C_SOURCES})

ADD_CUSTOM_COMMAND (TARGET kernel
        POST_BUILD
        COMMAND x86_64-elf-ld -T \"link.ld\" --orphan-handling=discard)
)
