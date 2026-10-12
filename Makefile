.PHONY: all build run clean

PLATFORM ?= linux

CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-unused-variable
IFLAGS = -I include

SRC = \
	src/camera.c \
	src/console.c \
	src/draw.c \
	src/main.c \
	src/mesh.c \
	src/settings.c \
	src/utils.c \
	src/view.c \
	src/visualizer.c

ifeq ($(PLATFORM), windows)
    CC = x86_64-w64-mingw32-gcc
    OUT = build/4vis_windows.exe
    LFLAGS = -Llib/windows -lraylib -lopengl32 -lwinmm -lgdi32 -lpthread -static -static-libgcc
    DFLAGS = -DPLATFORM_WINDOWS

else ifeq ($(PLATFORM), mac)
    CC = clang
    OUT = build/4vis_mac
    LFLAGS = -Llib/mac -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL
    DFLAGS = -DPLATFORM_MAC

else
    CC = gcc
    OUT = build/4vis_linux
    LFLAGS = -Llib/linux -lraylib -lpthread -lX11 -lm
    DFLAGS = -DPLATFORM_LINUX
endif

all: build run

build:
	@mkdir -p build
	@$(CC) $(CFLAGS) $(DFLAGS) $(SRC) $(IFLAGS) $(LFLAGS) -o $(OUT)

run:
	@./$(OUT)

clean:
	@rm -rf build/
