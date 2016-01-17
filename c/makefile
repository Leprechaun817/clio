flags = -Wall -Wextra -Wno-unused-parameter

all:
	@make example
	@make tests

clean:
	rm obj/*.o

example: obj/example.o obj/clio.o
	@mkdir -p bin
	cc ${flags} -o bin/example obj/example.o obj/clio.o

tests: obj/tests.o obj/clio.o
	@mkdir -p bin
	cc ${flags} -o bin/tests obj/tests.o obj/clio.o

obj/example.o: src/example.c src/clio.h
	@mkdir -p obj
	cc ${flags} -c -o obj/example.o src/example.c

obj/clio.o: src/clio.c
	@mkdir -p obj
	cc ${flags} -c -o obj/clio.o src/clio.c

obj/tests.o: src/tests.c src/clio.h
	@mkdir -p obj
	cc ${flags} -c -o obj/tests.o src/tests.c
