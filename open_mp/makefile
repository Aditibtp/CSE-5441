all: disposable persistent

disposable: sharma_aditi_disposable.c
	gcc -O3  sharma_aditi_disposable.c -o disposable -lrt -lpthread -fopenmp

persistent: sharma_aditi_persistent.c
	gcc -O3  sharma_aditi_persistent.c -o persistent -lrt -lpthread -fopenmp

clean:
	rm disposable persistent