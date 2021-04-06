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

#include "user.h"

#include "json.hpp"
#include "config.h"


bool running = true;


int initiate_socket(unsigned short listening_port){
	// Creating socket
	int listening_socket;
	if((listening_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("Error creating socket.\n");
		return -1;
	}
	else
		printf("Socket created.\n");

	// Binding
	struct sockaddr_in list_addr;
	list_addr.sin_family = AF_INET;
	list_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	list_addr.sin_port = htons(listening_port);
	memset(list_addr.sin_zero, '\0', sizeof(list_addr.sin_zero));

	if(bind(listening_socket, (struct sockaddr *) &list_addr, sizeof(list_addr)) < 0){
		printf("Error in binding.\n");
		return -1;
	}
	else
		printf("Socket binded.\n");

	// Listening
	if(listen(listening_socket, MAX_PENDING_CONNECTIONS) < 0){
		printf("Error in listening.\n");
		return -1;
	}
	else
		printf("Server is listening.\n");

	int opts = 1;
    if(setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, (char*) &opts, sizeof(opts)) < 0){
        printf("Error in options.\n");
        return -1;
    }

	return listening_socket;
}



void read_config(std::string config_path, int &command_channel_port, int &data_channel_port,
		std::vector<User> &users_list, std::vector<std::string> &forbidden_files_list){

	nlohmann::json config;

	std::ifstream file_reader(config_path, std::ifstream::binary);
	file_reader >> config;

	command_channel_port = config["commandChannelPort"];
	data_channel_port = config["dataChannelPort"];
	for(auto user : config["users"]){
		std::string username = user["user"];
		std::string password = user["password"];
		bool admin = false;
		if(user["admin"] == "true")
			admin = true;

		std::string str_size = user["size"];
		int size = std::atoi(str_size.c_str());

		users_list.push_back(User(username, password, admin, size));
	}
	for(auto file : config["files"]){
		forbidden_files_list.push_back(file);
	}
}

int accept_new_client(int listening_socket, int* clients_socket){
	struct sockaddr_in* caddr;
	struct sockaddr_in forlen;
	int len = sizeof(forlen);
	int* caddrlen = &len;

	int accepted_socket;
	if((accepted_socket = accept(listening_socket, (struct sockaddr *) caddr, (unsigned int *) caddrlen)) < 0){
		printf("New client connection failed.\n");
		return 1;
	}
	else
		printf("New client accepted.\n");

	char buf[BUF_SIZE];
	sprintf(buf, "Welcome\n");
	send(accepted_socket, &buf, strlen(buf), 0);

	for(int i = 0; i < MAX_CLIENTS; i++)
		if(clients_socket[i] == 0){
			clients_socket[i] = accepted_socket;
			break;
		}
	return 0;
}

int initiate_fds(fd_set &fds, int command_socket, int data_socket, int *clients_socket){
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	FD_SET(command_socket, &fds);
	FD_SET(command_socket, &fds);

	int fdmax = command_socket;
	for(int i = 0; i < MAX_CLIENTS; i++){
		if(clients_socket[i] == 0)
			continue;
		if(clients_socket[i] > fdmax)
			fdmax = clients_socket[i];
		FD_SET(clients_socket[i], &fds);
	}
	return fdmax;
}

int main(int argc, char* argv[]){

	int command_channel_port, data_channel_port;
	std::vector<User> users_list;
	std::vector<std::string> forbidden_files_list;

	read_config(argv[1], command_channel_port, data_channel_port, users_list, forbidden_files_list);

	int command_socket, data_socket;
	command_socket = initiate_socket(command_channel_port);
	data_socket = initiate_socket(data_channel_port);

	int clients_command_socket[MAX_CLIENTS], clients_data_socket[MAX_CLIENTS];
	memset(clients_command_socket, 0, sizeof(clients_command_socket));
	memset(clients_data_socket, 0, sizeof(clients_data_socket));

	fd_set readfds;
	char *buf[BUF_SIZE];
	while(running){
		int fdmax = initiate_fds(readfds, command_socket, data_socket, clients_command_socket);

		if(select(fdmax + 1, &readfds, NULL, NULL, NULL) == -1){
			printf("Select failed.\n");
			break;
		}

		// New connection
		if(FD_ISSET(command_socket, &readfds)){
			int status = accept_new_client(command_socket, clients_command_socket);
			continue;
		}
		if(FD_ISSET(data_socket, &readfds)){
			int status = accept_new_client(data_socket, clients_data_socket);
			continue;
		}

		// For STDIN
		if(FD_ISSET(STDIN_FILENO, &readfds)) {
			read(STDIN_FILENO, buf, sizeof(buf));
			continue;
		}

		// for(int i = 0; i < MAX_PLAYERS; i++){
		// 	if(FD_ISSET(clients_sockets[i], &fds)){
		// 		int bytes = recv(clients_sockets[i], &buf, sizeof(buf), 0);
		// 		if(bytes <= 0){
		// 			if(bytes < 0)
		// 				printf("Didn't receive message correctly.\n");
		// 			else if(bytes == 0){
		// 				printf("A player closed connection.\n");
		// 				clients_size--;
		// 				close(clients_sockets[i]);
		// 				clients_sockets[i] = 0;
		// 			}
		// 		}
		// 		else{
		// 			// Game ends
		// 			if(buf[0] == 'W'){
		// 				sprintf(buf, "Player %d won project %d!\n", i, client_project[i]);
		// 				print_terminal(buf);
		// 			}
		// 			// Choose a project
		// 			else{
		// 				int choice = atoi(buf);
		// 				sprintf(buf, "Client %d choosed %d project.\n", i, choice);
		// 				printf(buf);
		// 				// sprintf(buf, "%d\n", projectsno[2]);
		// 				// printf(buf);
		// 				// int result_of_assign = assign_client_to_project(i, choice, client_project, projectsno, clients_sockets, (int**) project_clients);
		// 				// projectsno[choice]++;
		// 				// sprintf(buf, "%d\n", projectsno[2]);
		// 				// printf(buf);
		// 				int client = i;
		// 				int project = choice;
		// 				client_project[client] = project;
		// 				//printf("project id %d\n", project);
		// 				sprintf(buf, "%d", projectsno[project]);
		// 				send(clients_sockets[client], &buf, strlen(buf), 0);

		// 				sprintf(buf, "Get id %d to client %d.\n", projectsno[project], client);
		// 				printf(buf);

		// 				project_clients[project][projectsno[project]] = client;
		// 				projectsno[project]++;
		// 				//sprintf(buf, "Get id %d to client %d.\n", projects[project], client);
		// 				//printf(buf);
		// 				if(projectsno[project] == PROJECT_PLAYERS){
		// 					//make_lobby(project, (int**) project_clients, clients_sockets);
		// 					int lobby_port = lobby_ports++;
		// 					sprintf(buf, "%d", lobby_port);
		// 					for(int i = 0; i < PROJECT_PLAYERS; i++){
		// 						int client_id = project_clients[project][i];
		// 						send(clients_sockets[client_id], &buf, strlen(buf), 0);
		// 					}

		// 					sprintf(buf, "New port for broadcasting sent to %d, %d, %d, %d, %d clients.\n", project_clients[project][0], project_clients[project][1], project_clients[project][2], project_clients[project][3], project_clients[project][4]);
		// 					printf(buf);
		// 					remove_project(project, projectsno);
		// 				}
		// 			}
		// 		}
		// 	}
		// }


	}

	return 0;
}