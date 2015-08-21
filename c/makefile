all:
	@mkdir -p bin obj
	@make example

example: obj/example.o obj/clio.o
	cc -o bin/example obj/example.o obj/clio.o

obj/example.o: src/example.c src/clio.h
	cc -c -o obj/example.o src/example.c

obj/clio.o: src/clio.c
	cc -c -o obj/clio.o src/clio.c
