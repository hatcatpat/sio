all: paint

paint: paint.c
	gcc -o paint paint.c -O3 -Wall -ansi -lSDL2 -DSIO_SDL -DSIO_IMPL

run: all
	./paint

clean:
	if [ -f paint ]; then rm paint; fi
