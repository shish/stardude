CC=gcc
CFLAGS=-std=c99 -I. -I.. -ggdb -pg `sdl-config --cflags`
LDFLAGS=-ldl -lGL -lGLU -pg `sdl-config --libs` -lSDL_image

all: stardude

stardude: stardude.o load.o init.o menu.o game.o stardude.h
	$(CC) stardude.o load.o init.o menu.o game.o $(CFLAGS) -o stardude $(LDFLAGS)

clean:
	rm -f stardude *.o
