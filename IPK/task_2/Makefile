all: server client RW.o

server : server.cc RW.o
	clang++ -std=gnu++11 -o server server.cc RW.o
client : client.cc  RW.o
	clang++ -std=gnu++11 -o client client.cc RW.o

RW.o : RW.cc
	clang++ -std=gnu++11 -c RW.cc RW.hh

clean:
	rm -f client server RW.o
