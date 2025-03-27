build:
	clang src/main.c -o bin/main.out

run:
	./bin/main.out

all:
	make build && make run