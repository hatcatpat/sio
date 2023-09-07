all: example

sprites.h: *.tga
	echo '#include "../sio.h"' > sprites.h
	../sioc/sioc *.tga >> sprites.h

example: example.c sprites.h
	gcc -o example example.c -O3 -Wall -ansi -lSDL2 -lm -DSIO_SDL -DSIO_IMPL

run: all
	./example

clean:
	rm example
