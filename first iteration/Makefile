all:
	gcc MAIN.c -o main
	nasm -f elf64 ASM.asm -o ASM.o
	ld -o ASM ASM.o
	./ASM