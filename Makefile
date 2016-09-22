
LIBS=-lSDL2 -lSDL2_image -lSDL2_ttf
SRCS=srcs/*.c
BIN=proprietary_ninja

all:
	gcc -g $(SRCS) $(LIBS) -o $(BIN)
