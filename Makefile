SRC=./src/*.c
FLAGS=-Wall -Wextra -Wpedantic
LIBS=-lraylib
EXE=main

main: $(SRC)
	gcc -o $(EXE) $(SRC) $(FLAGS) $(LIBS)
