.PHONY: all build run

all: build run

build:
	gcc -std=c99 \
		-Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-unused-variable \
			src/main.c \
			src/vis.c \
		-I include -L lib \
		-o build/4vis -lm -lraylib -lX11 -lpthread

run:
	./build/4vis
