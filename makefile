
CFLAGS = -Wall
OBJ = obj/core.o obj/editor.o obj/gameworld.o obj/graphic.o obj/input.o obj/mapbrowser.o

### Linux
CC = /usr/bin/gcc
OUTPUT = -o SDL_lin
LDFLAGS = -lm -lSDL_image -lSDL_ttf -lSDL_mixer `sdl-config --cflags`  `sdl-config --libs`

### Windows (Crosscompiler)
#CC = /usr/bin/i586-mingw32msvc-gcc
#OUTPUT = -o SDL_win.exe
#LDFLAGS = -lmingw32 -lm -Llib -lSDL_image -lSDL_ttf -lSDL_mixer -lSDLmain -lSDL

default:
	make clean
	make all

# link all together
all: $(OBJ)
	$(CC) $(CFLAGS) $(OUTPUT) src/main.c $(OBJ) $(LDFLAGS)
	
# object files
obj/core.o:
	$(CC) $(CFLAGS) -c src/core.c -o obj/core.o
obj/editor.o:
	$(CC) $(CFLAGS) -c src/editor.c -o obj/editor.o
obj/gameworld.o:
	$(CC) $(CFLAGS) -c src/gameworld.c -o obj/gameworld.o
obj/graphic.o:
	$(CC) $(CFLAGS) -c src/graphic.c -o obj/graphic.o
obj/input.o:
	$(CC) $(CFLAGS) -c src/input.c -o obj/input.o
obj/mapbrowser.o:
	$(CC) $(CFLAGS) -c src/mapbrowser.c -o obj/mapbrowser.o

clean:
	#rm -rf ./SDL_lin
	#rm -rf ./SDL_win.exe
	rm -rf ./obj/*
