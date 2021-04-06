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



int connect_to_server(unsigned short listening_port){
	// Creating socket
	int listening_socket;
	if((listening_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
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
	int bytes = recv(command_socket, &buf, sizeof(buf), 0);
	buf[bytes] = '\0';
	printf(buf);

	return 0;
}