CFLAGS = -std=c++11
CC = g++

Server: server.o user.o
	$(CC) $(CFLAGS) -o server server.o user.o

user.o: user.cpp
	$(CC) $(CFLAGS) -c user.cpp
server.o: server.cpp
	$(CC) $(CFLAGS) -c server.cpp
	
clean:
	rm -f core *.o 