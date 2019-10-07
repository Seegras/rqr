
CFLAGS=-Wall -O
CC=gcc

all: rqr

rqr: rqr.c
	$(CC) $(CFLAGS) -g rqr.c -o rqr -lm -lpopt

clean:
	rm *.o rqr