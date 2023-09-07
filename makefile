all: example

example: sio.h
	cd example && make example -B

run: all
	cd example && make run
