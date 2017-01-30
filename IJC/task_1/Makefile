NAME=prvocisla prvocisla-inline steg-decode

CC=gcc
CFLAGS= -O2 -std=c99 -lm -Wextra -Wall -pedantic 

%.o : %.c
	$(CC) $(CFLAGS) -c $<

all: $(NAME)

ppm.o: ppm.c ppm.h error.h
eratosthenes.o: eratosthenes.c eratosthenes.h error.h bit-array.h
error.o: error.c error.h
prvocisla.o: prvocisla.c eratosthenes.h bit-array.h error.h 
prvocisla-inline.o: prvocisla.c eratosthenes.h bit-array.h error.h 
	$(CC) $(CFLAGS) -DUSE_INLINE -c prvocisla.c -o prvocisla-inline.o
steg-decode.o: steg-decode.c eratosthenes.h ppm.h error.h bit-array.h


steg-decode: steg-decode.o eratosthenes.o error.o ppm.o
	$(CC) $(CFLAGS) steg-decode.o eratosthenes.o error.o ppm.o -o steg-decode

prvocisla: prvocisla.o eratosthenes.o bit-array.h error.o
	$(CC) $(CFLAGS) prvocisla.o eratosthenes.o bit-array.h error.o -o prvocisla

prvocisla-inline: prvocisla-inline.o eratosthenes.o bit-array.h error.o
	$(CC) $(CFLAGS) -DUSE_INLINE prvocisla-inline.o eratosthenes.o bit-array.h error.o -o prvocisla-inline

clean:
	rm *.o 
