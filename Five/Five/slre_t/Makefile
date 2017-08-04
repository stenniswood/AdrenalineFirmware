#PATH=/usr/local/CrossPack-AVR-20131216/bin:$PATH
CC             = g++

all:
	$(CC) -c -I../ slre.c slre_main.c protocol.c testing_exports.c 
	$(CC) -o test slre.o slre_main.o protocol.o testing_exports.o 
