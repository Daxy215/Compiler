@echo off
nasm -f win32 main.asm
gcc main.obj -o program.exe -lmsvcrt
program.exe
