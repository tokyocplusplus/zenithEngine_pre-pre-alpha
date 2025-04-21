all:
	gcc *.c -o main -lGL -lglfw -ldl -lm -O3
	nasm -f elf64 ASM.asm -o ASM.o
	gcc -o ASM ASM.o -nostartfiles
	./ASM

justasm:
	nasm -f elf64 ASM.asm -o ASM.o
	gcc -o ASM ASM.o -nostartfiles
	./ASM

# O2 = 4100 FPS average, 900 FPS 1% lows
# O3 = 4250 FPS average, 850 FPS 1% lows
# Os = 4150 FPS average, 838 FPS 1% lows
# NONE = about 2100 FPS average, 340 FPS 1% lows
bench: # benchmark
	gcc *.c -o main -lGL -lglfw -ldl -lm -O3
	gamemoderun mangohud ./main

ncd: # no charger development
	gcc *.c -o main -lGL -lglfw -ldl -lm -Os
	./main