all: bulk

sprites.h: *.tga
	../../sioc/sioc *.tga >> sprites.h

bulk: bulk.c sprites.h
	gcc -o bulk bulk.c -O3 -Wall -ansi -lSDL2 -DSIO_SDL -DSIO_IMPL

run: all
	./bulk

clean:
	rm bulk
