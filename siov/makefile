all: siov

siov: siov.c sprites.h
	gcc -o siov siov.c -O3 -Wall -ansi -lSDL2 -DSIO_SDL -DSIO_IMPL

run: all
	./siov

clean:
	rm siov
