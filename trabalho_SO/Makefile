all: folders server client

server: bin/monitor

client: bin/tracer

folders:
	@mkdir -p src obj bin tmp
	
bin/monitor: obj/monitor.o obj/auxiliares.o
	gcc -g obj/monitor.o obj/auxiliares.o -o bin/monitor

obj/monitor.o: src/monitor.c src/auxiliares.h
	gcc -Wall -g -c src/monitor.c -o obj/monitor.o

obj/auxiliares.o: src/auxiliares.c src/auxiliares.h
	gcc -Wall -g -c src/auxiliares.c -o obj/auxiliares.o

bin/tracer: obj/tracer.o obj/auxiliares.o
	gcc -g obj/tracer.o obj/auxiliares.o -o bin/tracer

obj/tracer.o: src/tracer.c src/auxiliares.h
	gcc -Wall -g -c src/tracer.c -o obj/tracer.o

clean:
	rm -f obj/* tmp/* bin/{tracer,monitor}
