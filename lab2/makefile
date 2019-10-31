all: disposable persistent

disposable: sharma_aditi_disposable.c
	gcc -O3  sharma_aditi_disposable.c -o disposable -lrt -lpthread

persistent: sharma_aditi_persistent.c
	gcc -O3  sharma_aditi_persistent.c -o persistent -lrt -lpthread

clean:
	rm disposable persistent