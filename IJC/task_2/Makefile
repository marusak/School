#Riesenie IJC-DU2, priklad b),10.4.2015
#Autor: Matej Marusak, xmarus06, VUT FIT
#Makefile

NAME=wordcount wordcount-dynamic tail tail2
LIBFILES=htab_init.o htab_lookup.o hash_function.o htab_clear.o htab_free.o htab_remove.o htab_foreach.o htab_statistics.o
CC=gcc
CPPC=g++

CFLAGS=-std=c99 -pedantic -Wall
CPPFLAGS= -std=c++11 -Wall -pedantic

all: $(NAME)

tail:tail.c
	$(CC) $(CFLAGS) -o tail tail.c

tail2:tail2.cc
	$(CPPC) $(CPPFLAGS) -o tail2 tail2.cc

wordcount:wordcount.o io.o libhtable.a
	$(CC) $(CFLAGS) -o wordcount -static io.o wordcount.o libhtable.a

wordcount-dynamic:wordcount.o io.o libhtable.so
	$(CC) $(CFLAGS) -o wordcount-dynamic io.o wordcount.o libhtable.so

wordcount.o:wordcount.c htable.h
	$(CC) $(CFLAGS) -c wordcount.c -o wordcount.o

#kniznice
libhtable.a:$(LIBFILES)
	ar crs libhtable.a $(LIBFILES)

libhtable.so:$(LIBFILES)
	gcc $(CFLAGS) -shared -fPIC -o libhtable.so $(LIBFILES)

#binarky
io.o:
	$(CC) $(CFLAGS) -c io.c -o io.o

hash_function.o:hash_function.c hash_function.h
	$(CC) $(CFLAGS) -fPIC -c hash_function.c

htab_init.o:htab_init.c htable.h
	$(CC) $(CFLAGS) -fPIC -c htab_init.c -o htab_init.o

htab_lookup.o:htab_lookup.c htable.h
	$(CC) $(CFLAGS) -fPIC -c htab_lookup.c -o htab_lookup.o

htab_clear.o:htab_clear.c htable.h
	$(CC) $(CFLAGS) -fPIC -c htab_clear.c -o htab_clear.o

htab_free.o:htab_free.c htable.h
	$(CC) $(CFLAGS) -fPIC -c htab_free.c -o htab_free.o

htab_foreach.o:htab_foreach.c htable.h
	$(CC) $(CFLAGS) -fPIC -c htab_foreach.c -o htab_foreach.o

htab_remove.o:htab_remove.c htable.h
	$(CC) $(CFLAGS) -fPIC -c htab_remove.c -o htab_remove.o

htab_statistics.o:htab_statistics.c htable.h
	$(CC) $(CFLAGS) -fPIC -c htab_statistics.c -o htab_statistics.o
