# Makefile for SDL app

# Compiler and linker flags
CFLAGS = -I src/include -L src/lib
LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_TTF -lSDL2_image -lSDL2_mixer

# Default target
all: main

# Build target for app
main: main.c auth.c
	gcc $(CFLAGS) -o main main.c auth.c  $(LIBS)

# Build target for debugging
debug: CFLAGS += -g
debug: main

# Target to clean up
clean:
	rm -f main
