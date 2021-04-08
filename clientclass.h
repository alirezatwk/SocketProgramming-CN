#ifndef CLIENTCLASS_H
#define CLIENTCLASS_H

#include <string>
#include <iostream>
#include <vector>
#include <cstring>
#include <stdexcept>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "user.h"
#include "str.h"

#define NO_USERNAME 0
#define USERNAME 1
#define LOGEDIN 2

#define BUF_SIZE 300

class Client{
public:
	// TODO sockets = 0 in begin

	Client();

	void run(Str_Handler &str_handler, std::vector<User> &users_list, std::vector<std::string> &forbidden_files_list);

	int get_status();

	// std::string Client::exec(const char* cmd);


	int command_socket, data_socket;
private:
	std::string pwd;
	int status;

	
	User user;
};


#endif