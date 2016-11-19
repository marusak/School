imapcl : imapcl.o imap.o
	g++ -std=gnu++11 -Wall -Wextra -o imapcl imapcl.o imap.o -L/usr/lib -lcrypto -lssl

imap.o : imap.cc imap.hh
	g++ -std=gnu++11 -Wall -Wextra -c -o imap.o imap.cc

imapcl.o : imapcl.cc imap.hh
	g++ -std=gnu++11 -Wall -Wextra -c -o imapcl.o imapcl.cc

clean:
	rm *.o imapcl xmarus06.tar

tar:
	tar -cf xmarus06.tar Makefile imapcl.cc imap.cc imap.hh README manual.pdf
