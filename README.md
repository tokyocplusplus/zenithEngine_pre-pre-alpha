# zenithEngine_pre-pre-alpha
soon to be beta??
compile command in my makefile {
all:
	gcc MAIN.c -o main
	nasm -f elf32 ASM.asm -o ASM.o
	ld -m elf_i386 -s -o ASM ASM.o
	./ASM
}
# NOTES {
### 1. Basic cube render and make it spin
### 2. Using GLM specifically, make a first person controller that is similar to unitys first person editor
### 3. Simplify it spinning using GUI, like ImGui or something similar, consider RayGUI
### 4. Add an in game object editor
# }
