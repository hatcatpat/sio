all: sioc

sioc: *.c
	gcc -o sioc sioc.c -O3 -Wall -ansi

run: all
	./sioc

clean:
	if [ -f sioc ]; then rm sioc; fi
