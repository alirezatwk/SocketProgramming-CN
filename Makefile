CFLAGS = -std=c++11
CC = g++

All: Server Client

Server: server.o user.o
	$(CC) $(CFLAGS) -o server server.o user.o

Client: client.o
	$(CC) $(CFLAGS) -o client client.o

user.o: user.cpp
	$(CC) $(CFLAGS) -c user.cpp
server.o: server.cpp
	$(CC) $(CFLAGS) -c server.cpp
client.o: client.cpp
	$(CC) $(CFLAGS) -c client.cpp

clean:
	rm -f core *.o 