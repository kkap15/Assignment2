CFLAGS=-std=gnu99 -Wall -g -pedantic
.PHONY= clean all
.DEFAULT_GOAL := all
.PHONY = all

all: server client clientbot
	gcc $(CFLAGS) utils.o server.o communication.o -o server
	gcc $(CFLAGS) utils.o client.o communication.o -o client
	gcc $(CFLAGS) utils.o clientbot.o communication.o -o clientbot

client: utils communication client.o
		gcc $(CFLAGS) -c client.c -o client.o
		
clientbot: utils communication clientbot.o
		gcc $(CFLAGS) -c clientbot.c -o clientbot.o

server: utils communication server.o
		gcc $(CFLAGS) -c server.c -o server.o

utils: utils.o
		gcc $(CFLAGS) -c utils.c -o utils.o

communication: communication.o
		gcc $(CFLAGS) -c communication.c -o communication.o

clean:
		rm *.o
