.PHONY: all build run clean

PLATFORM ?= linux

STD = -std=c99
WFLAGS = -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-unused-variable

SRC = src/main.c src/vis.c

ifeq ($(PLATFORM), windows)
    CC = x86_64-w64-mingw32-gcc
    OUT = build/4vis_windows.exe
    LFLAGS = -Llib/windows -lraylib -lopengl32 -lwinmm -lgdi32 -lpthread -static -static-libgcc
    CFLAGS = -DPLATFORM_WINDOWS

else ifeq ($(PLATFORM), mac)
    CC = clang
    OUT = build/4vis_mac
    LFLAGS = -Llib/mac -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL
    CFLAGS = -DPLATFORM_MAC

else
    CC = gcc
    OUT = build/4vis_linux
    LFLAGS = -Llib/linux -lraylib -lpthread -lX11 -lm
    CFLAGS = -DPLATFORM_LINUX
endif

all: build run

build:
	@mkdir -p build
	$(CC) $(STD) $(WFLAGS) $(CFLAGS) $(SRC) -Iinclude $(LFLAGS) -o $(OUT)

run:
	@./$(OUT)

clean:
	rm -rf build/
