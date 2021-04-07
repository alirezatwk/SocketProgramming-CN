CFLAGS = -std=c++11
CC = g++

All: Server Client

Server: server.o user.o str.o
	$(CC) $(CFLAGS) -o server server.o user.o str.o

Client: client.o
	$(CC) $(CFLAGS) -o client client.o

user.o: user.cpp
	$(CC) $(CFLAGS) -c user.cpp
server.o: server.cpp
	$(CC) $(CFLAGS) -c server.cpp
client.o: client.cpp
	$(CC) $(CFLAGS) -c client.cpp
str.o: str.cpp
	$(CC) $(CFLAGS) -c str.cpp

clean:
	rm -f core *.o 