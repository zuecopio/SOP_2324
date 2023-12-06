bin/hilos: lec-esc-hilos.c
	mkdir -p bin
	gcc -o bin/le-hilos lec-esc-hilos.c -lpthread
	
hilos: bin/hilos
	./bin/le-hilos

bin/semaforos: lec-esc-semaforos.c
	mkdir -p bin
	gcc -o bin/le-semaforos lec-esc-semaforos.c -lpthread
	
semaforos: bin/semaforos
	./bin/le-semaforos

bin/mutex: lec-esc-mutex.c
	mkdir -p bin
	gcc -o bin/le-mutex lec-esc-mutex.c -lpthread
	
mutex: bin/mutex
	./bin/le-mutex

bin/monitores: lec-esc-monitores.c
	mkdir -p bin
	gcc -o bin/le-monitores lec-esc-monitores.c -lpthread
	
monitores: bin/monitores
	./bin/le-monitores

	
clean:
	rm -r bin 
