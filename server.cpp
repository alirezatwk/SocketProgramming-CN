#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include "json.hpp"

// int initiate_sockets(unsigned short listening_port){
// 	// Creating socket
// 	int listening_socket;
// 	if((listening_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
// 		print_log("Error creating socket.\n");
// 	else
// 		print_log("Socket created.\n");

// 	// Binding
// 	struct sockaddr_in list_addr;
// 	list_addr.sin_family = AF_INET;
// 	list_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
// 	list_addr.sin_port = htons(listening_port);
// 	memset(list_addr.sin_zero, '\0', sizeof(list_addr.sin_zero));

// 	if(bind(listening_socket, (struct sockaddr *) &list_addr, sizeof(list_addr)) < 0)
// 		print_log("Error in binding.\n");
// 	else
// 		print_log("Socket binded.\n");

// 	// Listening
// 	if(listen(listening_socket, MAX_PENDING_CONNECTIONS) < 0)
// 		print_log("Error in listening.\n");
// 	else
// 		print_log("Server is listening.\n");

// 	int opts = 1;
//     if(setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, (char*) &opts, sizeof(opts)) < 0)
//         print_log("Error in options.\n");

// 	return listening_socket;
// }

class User{
public:
	User(std::string username, std::string password, bool admin, int size): username(username), password(password), admin(admin), size(size) {}

	std::string get_username(){
		return username;
	}
	std::string get_password(){
		return password;
	}
	bool is_admin(){
		return admin;
	}
	int get_size(){
		return size;
	}

private:
	std::string username, password;
	bool admin;
	int size;
};

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
	// initiate_sockets()

	std::cout << command_channel_port << ' ' << data_channel_port << '\n';
	for(auto user : users_list){
		std::cout << user.get_username() << ' ' << user.get_password() << ' ' << user.is_admin() << ' ' << user.get_size() << '\n';
	}
	for(auto file : forbidden_files_list){
		std::cout << file << '\n';
	}
	return 0;
}