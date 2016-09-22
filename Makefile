
LIBS=-lSDL2 -lSDL2_image
SRCS=main.c
BIN=proprietary_ninja

all:
	gcc -g $(SRCS) $(LIBS) -o $(BIN)

