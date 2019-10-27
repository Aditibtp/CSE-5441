all: lab1

lab1: program_1.c
	gcc -O3  program_1.c -o program_1 -lrt 

clean:
	rm program_1