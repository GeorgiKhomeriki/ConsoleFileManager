CC=gcc

cfm: main.c
	${CC} main.c -o cfm -lncurses

run: main.c
	${CC} main.c -o cfm -lncurses
	./cfm

