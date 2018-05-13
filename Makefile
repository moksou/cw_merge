
WORKDIR = `pwd`

CC = gcc
CFLAGS = -I. -Wall -std=c99 -std=gnu99
LDFLAGS = 
SOURCES = main.c misc.c lump.c textures.c wad.c 
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = cw_merge


all: 
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o $(EXECUTABLE)
