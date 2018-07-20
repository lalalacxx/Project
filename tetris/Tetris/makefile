.PHONY:clean

OBJ = obj/def.o obj/tetris.o
CFLAGS= -Iinclude -g
all:tetris

tetris:$(OBJ)
	gcc  $(CFLAGS) $^ -o bin/$@ -Iinclude -std=gnu99

obj/%.o:src/%.c
	gcc $(CFLAGS) -c $< -o $@ -Iinclude -std=gnu99

clean:
	rm -f obj/*
