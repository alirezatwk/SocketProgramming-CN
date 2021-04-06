#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include "user.h"

#include "json.hpp"
#include "config.h"



// int initiate_sockets(unsigned short command_channel_port, unsigned short data_channel_port, int &command_socket, int &data_socket){
// 	// Creating socket
// 	if((command_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
// 		printf("Error creating command socket.\n");
// 	else
// 		printf("Command socket created.\n");

// 	// Binding
// 	struct sockaddr_in list_addrr;
// 	list_addrr.sin_family = AF_INET;
// 	list_addrr.sin_addr.s_addr = inet_addr(SERVER_IP);
// 	list_addrr.sin_port = htons(command_channel_port);
// 	memset(list_addrr.sin_zero, '\0', sizeof(list_addrr.sin_zero));

// 	if(bind(command_socket, (struct sockaddr *) &list_addrr, sizeof(list_addrr)) < 0)
// 		printf("Error in binding.\n");
// 	else
// 		printf("Socket binded.\n");

// 	// Listening
// 	if(listen(command_socket, MAX_PENDING_CONNECTIONS) < 0)
// 		printf("Error in listening.\n");
// 	else
// 		printf("Server is listening.\n");

// 	int opts = 1;
//     if(setsockopt(command_socket, SOL_SOCKET, SO_REUSEADDR, (char*) &opts, sizeof(opts)) < 0)
//         printf("Error in options.\n");

// 	return 0;
// }



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



int main(int argc, char* argv[]){

	int command_channel_port, data_channel_port;
	std::vector<User> users_list;
	std::vector<std::string> forbidden_files_list;
	
	read_config(argv[1], command_channel_port, data_channel_port, users_list, forbidden_files_list);

	// int 
	// initiate_sockets();


	std::cout << command_channel_port << ' ' << data_channel_port << '\n';
	for(auto user : users_list){
		std::cout << user.get_username() << ' ' << user.get_password() << ' ' << user.is_admin() << ' ' << user.get_size() << '\n';
	}
	for(auto file : forbidden_files_list){
		std::cout << file << '\n';
	}
	return 0;
}