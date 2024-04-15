# Globales
CC = gcc
MODULE = io


# SRC -> find ./src -type f -iname "*.c" -exec basename {} \; (Da una lista de *.c)
SRC = $(shell find ./src -type f -iname "*.c" -exec basename {} \;)
OBJ = $(SRC:%.c=./obj/%.o) #De la variable SRC agarra todo lo que sea *.c  y lo pone en la carpeta obj/*.o
SHARED = $(shell find ../shared/obj/*.o)

LINK = -lpthread -lcommons -lm
CFLAGS = -c
BUILD_DEBUG_FLAG = 


build: makefolder makeshared $(OBJ)
	$(CC) $(BUILD_DEBUG_FLAG) -o $(MODULE).out $(MODULE).c $(OBJ) $(SHARED) $(LINK) 

debug: CFLAGS := $(CFLAGS) -g
debug: makeshared_debug build
debug: BUILD_DEBUG_FLAG := -g

./obj/%.o: ./src/%.c # REGLA # Este link no funca
	$(CC) $(CFLAGS) -c $< -o $@ $(LINK) 

makefolder:  # Crea carpeta obj
	if [ ! -d ./obj ]; then mkdir obj; fi

makeshared:
	make -C ../shared

makeshared_debug:
	 make debug -C ../shared

valgrind: build
	valgrind --leak-check=full --show-leak-kinds=all --log-file=valgrind.log ./$(OUTFILE).out


# Clean

clean:
	rm -rf obj
	rm -rf *.out
	rm -rf *.log
	make clean -C ../shared