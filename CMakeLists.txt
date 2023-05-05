cmake_minimum_required(VERSION 3.0)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project("knkOs")

include_directories("${PROJECT_SOURCE_DIR}/src")

set (CMAKE_C_FLAGS "-Ttext 0x8000 -I${PROJECT_SOURCE_DIR}/include -ffreestanding -nolibc -mno-red-zone -m64")
set (CMAKE_EXE_LINKER_FLAGS "-T\"link.ld\"")

FILE (GLOB_RECURSE kernel_C_SOURCES "*.c")

ADD_LIBRARY (kernel STATIC
                ${kernel_C_SOURCES})

ADD_CUSTOM_COMMAND (TARGET kernel
        POST_BUILD
        COMMAND x86_64-elf-ld -T \"link.ld\")

