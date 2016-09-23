
LIBS=-lm -lSDL2 -lSDL2_image -lSDL2_ttf
SRCS=srcs/*.c
BIN=proprietary_ninja

all:
	gcc -g -O2 $(SRCS) $(LIBS) -o $(BIN)
