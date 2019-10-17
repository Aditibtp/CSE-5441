all: hellothreads

hellothreads: program_1.c
	gcc program_1.c -o program_1 -lpthread

clean:
	rm hellothreads