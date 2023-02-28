CFLAGS = -lSDL2
EXECUTABLE_NAME = main
SRC = src
BIN = bin

all: $(BIN)/main

$(BIN)/main: $(SRC)/main.c
	mkdir -p $(BIN)
	gcc $(CFLAGS) $(SRC)/main.c -o $(BIN)/main
