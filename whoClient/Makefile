OBJS = main.o input.o
SOURCE = main.c input.c
HEADER = structs.h functions.h
OUT = whoClient
CC = gcc
FLAG = -c

all: $(OBJS)
	$(CC) $(OBJS) -o $(OUT) -lpthread

main.o: main.c
	$(CC) $(FLAG) main.c

input.o: input.c
	$(CC) $(FLAG) input.c

clean:
	rm -f $(OBJS) $(OUT)
