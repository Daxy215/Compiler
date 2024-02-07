@echo off
nasm -f win32 main.asm
gcc main.obj -o program.exe -I C:\Users\smsmk\Documents\Anix\Anix\glm\glm 
program.exe
