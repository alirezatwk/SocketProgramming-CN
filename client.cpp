#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>

#include "config.h"

bool running = true;

int connect_to_server(unsigned short listening_port){
	// Creating socket
	int listening_socket;
	if((listening_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		printf("Error creating socket.\n");
		return -1;
	}
	else
		printf("Socket created.\n");

	// Connecting
	struct sockaddr_in list_addr;
	list_addr.sin_family = AF_INET;
	list_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	list_addr.sin_port = htons(listening_port);
	memset(list_addr.sin_zero, '\0', sizeof(list_addr.sin_zero));

	if(connect(listening_socket, (struct sockaddr*) &list_addr, sizeof(list_addr)) < 0){
        printf("Error in connecting to server.\n");
        return -1;
	}
    else
    	printf("Socket connected correctly.\n");

    return listening_socket;
}

int main(int argc, char* argv[]){
	int command_socket = connect_to_server(COMMAND_CHANNEL_PORT);
	int data_socket = connect_to_server(DATA_CHANNEL_PORT);

	char buf[BUF_SIZE];
	fd_set readfds;

	while(running){

		FD_ZERO(&readfds);
		FD_SET(STDIN_FILENO, &readfds);
		FD_SET(command_socket, &readfds);
		FD_SET(data_socket, &readfds);

		int fdmax = std::max(command_socket, data_socket);
		if(select(fdmax + 1, &readfds, NULL, NULL, NULL) == -1){
			printf("Select failed.\n");
			break;
		}


		if(FD_ISSET(command_socket, &readfds)){
			int bytes = recv(command_socket, &buf, sizeof(buf), 0);
			buf[bytes] = '\0';
			printf(buf);
			if(buf[0] == '2' && buf[1] == '2' && buf[2] == '1')
				running = false;
			continue;
		}
		if(FD_ISSET(data_socket, &readfds)){
			int bytes = recv(data_socket, &buf, sizeof(buf), 0);
			buf[bytes] = '\0';
			printf(buf);
			continue;
		}

		// For STDIN
		if(FD_ISSET(STDIN_FILENO, &readfds)) {
			int bytes = read(STDIN_FILENO, buf, sizeof(buf));
			buf[bytes - 1] = '\0';
			// printf("%s\n", buf);
			send(command_socket, &buf, strlen(buf), 0);
			continue;
		}
		
	}

	close(command_socket);
	close(data_socket);

	return 0;
}