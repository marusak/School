imapcl : imapcl.o imap.o
	clang++ -std=gnu++11 -Wall -Wextra -o imapcl imapcl.o imap.o -L/usr/lib -lcrypto -lssl

imap.o : imap.cc imap.hh
	clang++ -std=gnu++11 -Wall -Wextra -c -o imap.o imap.cc

imapcl.o : imapcl.cc imap.hh
	clang++ -std=gnu++11 -Wall -Wextra -c -o imapcl.o imapcl.cc

