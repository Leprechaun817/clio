# --------------------------------------------------------------------------
# Make variables.
# --------------------------------------------------------------------------

CFLAGS=-Wall -Wextra --std=gnu11

# --------------------------------------------------------------------------
# Targets.
# --------------------------------------------------------------------------

example: src/example.c src/clio.c src/clio.h
	@mkdir -p bin
	$(CC) $(CFLAGS) -o bin/example src/example.c src/clio.c

tests: src/tests.c src/clio.c src/clio.h
	@mkdir -p bin
	$(CC) $(CFLAGS) -Wno-unused-parameter -o bin/tests src/tests.c src/clio.c
	./bin/tests

test:
	@make tests

clean:
	rm -f ./src/*.o
	rm -f ./bin/tests
	rm -f ./bin/example
