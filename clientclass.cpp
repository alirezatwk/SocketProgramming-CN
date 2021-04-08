#include "clientclass.h"

std::string exec(const char* cmd) {
    char buffer[128];
    printf("exec get: %s", cmd);
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
    result[result.size() - 1] = '\0';
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
	char buf[BUF_SIZE * 20];
	char buf2[BUF_SIZE];

	// TODO: check access.

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
						pwd = exec("pwd");
					}
					break;
				}
			if(find == false){
				sprintf(buf, "430: Invalid username or password\n");
				status = NO_USERNAME;
			}
		}
	}
	else if(str_handler.get_command_code() == 9){ // doesn't send all
		sprintf(buf, "214\nUSER[name], Its argument is used to specify the user string. It is used foruser authentication.\nPASS[pass], Its password of account.\nPWD, Show you your path.\nMKD[path], Make directory with path name.\nDELE[option, file/directory], Delete file/directory with specific options.\nLS, Show you files and directories in your path.\nCWD[path], Change directory to path.\nRENAME[name, new_name], Change name to new_name.\nRETR[name], Download filename.\nHELP, Show help of commands.\nQUIT, Quit from server.\n");
	}
	else if(str_handler.get_command_code() == 10)
		sprintf(buf, "221: Successful Quit.\n");
	else if(str_handler.get_command_code() == -1)
		sprintf(buf, "501: Syntax error in parameters or arguments.\n");
	else{
		if(status != LOGEDIN)
			sprintf(buf, "332: Need account for login.\n");
		else{
			if(str_handler.get_command_code() == 2){
				strcpy(buf2, pwd.c_str());
				sprintf(buf, "257: %s\n", buf2);
			}
			else if(str_handler.get_command_code() == 3){
				sprintf(buf, "cd %s && mkdir %s", pwd.c_str(), str_handler.get_attr1().c_str());
				exec(buf);
				sprintf(buf, "257: %s created.\n", str_handler.get_attr1().c_str());
			}
			else if(str_handler.get_command_code() == 4 || str_handler.get_command_code() == 11){  // -f, -d

				// TODO: not existed or not accept.
				sprintf(buf, "cd %s && rm -rf %s", pwd.c_str(), str_handler.get_attr1().c_str());
				exec(buf);
				sprintf(buf, "250: %s deleted.\n", str_handler.get_attr1().c_str());
			}
			else if(str_handler.get_command_code() == 5){
				std::cout << "ls" << std::endl;

			}
			else if(str_handler.get_command_code() == 6){
				// TODO: Check existence.
				sprintf(buf, "cd %s && cd %s && pwd", pwd.c_str(), str_handler.get_attr1().c_str());
				pwd = exec(buf);
				sprintf(buf, "250: Successful change.\n");
			}
			else if(str_handler.get_command_code() == 7){
				// TODO: Check existance
				sprintf(buf, "cd %s && mv %s %s", pwd.c_str(), str_handler.get_attr1().c_str(), str_handler.get_attr2().c_str());
				exec(buf);
				sprintf(buf, "250: Successful change.\n");
			}
			else if(str_handler.get_command_code() == 8)
				std::cout << "retr" << std::endl;
		}
	}
	
	send(command_socket, &buf, strlen(buf), 0);
}


int Client::get_status(){
	return status;
}