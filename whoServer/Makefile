OBJS = main.o readStats.o commands.o buffer.o
SOURCE = main.c readStats.c commands.c buffer.c
HEADER = structs.h functions.h
OUT = whoServer
CC = gcc
FLAG = -c

all: $(OBJS)
	$(CC) $(OBJS) -o $(OUT) -lpthread

main.o: main.c
	$(CC) $(FLAG) main.c

readStats.o: readStats.c
	$(CC) $(FLAG) readStats.c

commands.o: commands.c
	$(CC) $(FLAG) commands.c

buffer.o: buffer.c
	$(CC) $(FLAG) buffer.c

clean:
	rm -f $(OBJS) $(OUT)
