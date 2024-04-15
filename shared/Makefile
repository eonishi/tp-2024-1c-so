# Globales
CC = gcc

SRC = $(shell find ./src -type f -iname "*.c" -exec basename {} \;)
OBJ = $(SRC:%.c=./obj/%.o) #De la variable SRC agarra todo lo que sea *.c  y ponelo en la carpeta obj/*.o
LINK = -lpthread -lcommons -lm

build: makefolder $(OBJ) $(LINK)


CFLAGS = -c

debug: CFLAGS := $(CFLAGS) -g
debug: build

./obj/%.o: ./src/%.c # REGLA
	$(CC) $(CFLAGS) $< -o $@ $(LINK)

makefolder:  # Crea carpeta obj
	if [ ! -d ./obj ]; then mkdir obj; fi

clean:
	rm -rf obj
	rm -rf *.out
	rm -rf *.log