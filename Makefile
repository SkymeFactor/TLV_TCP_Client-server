CC=g++
CFLAGS=-std=c++17 -lpthread

all: server client

server:
	$(CC) main_server.cpp -o server.x86_64 $(CFLAGS) 

client:
	$(CC) main_client.cpp -o client.x86_64 $(CFLAGS) 

clean:
	rm -rf *.x86_64 *.o