#include "clientclass.h"

std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;

        }
    } catch (...) {
        pclose(pipe);
        throw;
    }

    pclose(pipe);
    std::cout << "result is:" << result << std::endl;
    return result;
}

bool exists(std::string username, std::vector<User> &users_list){
	for(auto user_from_list : users_list)
		if(user_from_list.get_username() == username)
			return true;
	return false;
}

Client::Client(): command_socket(0), data_socket(0), user(User()), pwd("") {}

//"user" , "pass" , "pwd" , "mkd" , "dele" , "ls" , "cwd" , "rename" , "retr" , "help" , "quit"

void Client::run(Str_Handler &str_handler, std::vector<User> &users_list, std::vector<std::string> &forbidden_files_list){
	char buf[BUF_SIZE];
	char buf2[BUF_SIZE];
	if(str_handler.get_command_code() == 0){
		status = USERNAME;
		user.set_username(str_handler.get_attr1());
		sprintf(buf, "331: User name okay, need password.\n");
	}
	else if(str_handler.get_command_code() == 1){
		if(status != USERNAME)
			sprintf(buf, "503: Bad sequence of commands.\n");
		else{
			bool find = false;
			for(auto user_from_list : users_list)
				if(user_from_list.get_username() == user.get_username()){
					if(user_from_list.get_password() == str_handler.get_attr1()){
						sprintf(buf, "230: User logged in, proceed. Logged out if appropriate.\n");
						status = LOGEDIN;
						find = true;
					}
					break;
				}
			if(find == false){
				sprintf(buf, "430: Invalid username or password\n");
				status = NO_USERNAME;
			}
		}
	}

	else if(str_handler.get_command_code() == 2){
		if(pwd == ""){
			pwd = exec("pwd");
			std::cout << "pwd is:" << pwd << std::endl;
		}
		strcpy(buf2, pwd.c_str());
		sprintf(buf, "257: %s\n", buf2);
	}
	else if(str_handler.get_command_code() == 3){
		sprintf(buf, "cd %s && mkdir ", pwd);
		exec(buf);
	}
	else if(str_handler.get_command_code() == 4)
		std::cout << "dele" << std::endl;
	else if(str_handler.get_command_code() == 5)
		std::cout << "ls" << std::endl;
	else if(str_handler.get_command_code() == 6)
		std::cout << "cwd" << std::endl;
	else if(str_handler.get_command_code() == 7)
		std::cout << "rename" << std::endl;
	else if(str_handler.get_command_code() == 8)
		std::cout << "retr" << std::endl;
	else if(str_handler.get_command_code() == 9)
		std::cout << "help" << std::endl;
	else if(str_handler.get_command_code() == 10)
		std::cout << "quit" << std::endl;
	else if(str_handler.get_command_code() == -1)
		std::cout << "Bad Command" << std::endl;
	send(command_socket, &buf, strlen(buf), 0);
}


int Client::get_status(){
	return status;
}