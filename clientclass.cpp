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

bool exists(std::string filename, std::vector<std::string> &forbidden_files_list){
	for(auto file : forbidden_files_list)
		if(file == filename)
			return true;
	return false;
}

Client::Client(): command_socket(0), data_socket(0), user(User()), pwd("") {}

void Client::log(std::string str){
	std::ofstream fout;
	fout.open("server.log", std::ios::app);

	time_t date = time(0);
	char* dt = ctime(&date);
	dt[strlen(dt) - 1] = '\0';
	fout << '[' << dt << " User: " << user.get_username() << "] " <<  str << '\n';

	fout.close();
}

void Client::run(Str_Handler &str_handler, std::vector<User> &users_list, std::vector<std::string> &forbidden_files_list){
	char buf[BUF_SIZE * 100];
	char buf2[BUF_SIZE];

	if(str_handler.get_command_code() == 0){ // user
		status = USERNAME;
		user.set_username(str_handler.get_attr1());
		sprintf(buf, "331: User name okay, need password.\n");
	}
	else if(str_handler.get_command_code() == 1){ // pass
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
						log("Login.");
						user = user_from_list;
					}
					break;
				}
			if(find == false){
				sprintf(buf, "430: Invalid username or password\n");
				status = NO_USERNAME;
			}
		}
	}
	else if(str_handler.get_command_code() == 9){ // doesn't send all // help
		sprintf(buf, "214\nUSER[name], Its argument is used to specify the user string. It is used foruser authentication.\nPASS[pass], Its password of account.\nPWD, Show you your path.\nMKD[path], Make directory with path name.\nDELE[option, file/directory], Delete file/directory with specific options.\nLS, Show you files and directories in your path.\nCWD[path], Change directory to path.\nRENAME[name, new_name], Change name to new_name.\nRETR[name], Download filename.\nHELP, Show help of commands.\nQUIT, Quit from server.\n");
		log("Ask for help.");
	}
	else if(str_handler.get_command_code() == 10){ // quit
		sprintf(buf, "221: Successful Quit.\n");
		log("Quit.");
	}
	else if(str_handler.get_command_code() == -1){ // not valid
		sprintf(buf, "501: Syntax error in parameters or arguments.\n");
		log("Send something that is not valid.");
	}
	else{
		if(status != LOGEDIN)
			sprintf(buf, "332: Need account for login.\n");
		else{
			if(str_handler.get_command_code() == 2){ // pwd
				strcpy(buf2, pwd.c_str());
				sprintf(buf, "257: %s\n", buf2);
				log("Ask for pwd.");
			}
			else if(str_handler.get_command_code() == 3){ //mkd
				sprintf(buf, "cd %s && mkdir %s", pwd.c_str(), str_handler.get_attr1().c_str());
				exec(buf);
				sprintf(buf, "257: %s created.\n", str_handler.get_attr1().c_str());
				log("Ask for making directory.");
			}
			else if(str_handler.get_command_code() == 5){ // ls
				sprintf(buf, "cd %s && ls", pwd.c_str());
				std::string result = exec(buf);
				sprintf(buf, "%s\n", result.c_str());
				send(data_socket, &buf, strlen(buf), 0);

				sprintf(buf, "226: List transfer done.\n");
				log("Ask for list of files.");
			}
			else if(str_handler.get_command_code() == 6){ // cwd
				// TODO: Check existence.
				sprintf(buf, "cd %s && cd %s && pwd", pwd.c_str(), str_handler.get_attr1().c_str());
				pwd = exec(buf);
				sprintf(buf, "250: Successful change.\n");
				std::string log_str = "Ask for changing path to " + str_handler.get_attr1();
				log(log_str);
			}
			else if(str_handler.get_command_code() == 11){  // -d

				// TODO: not existed or not accept.
				sprintf(buf, "cd %s && rm -rf %s", pwd.c_str(), str_handler.get_attr1().c_str());
				exec(buf);
				sprintf(buf, "250: %s deleted.\n", str_handler.get_attr1().c_str());
				std::string log_str = "Ask for deleting directory of " + str_handler.get_attr1();
				log(log_str);
			}
			
			else{
				//std::cout << "file: " << str_handler.get_attr1() << " and exists result is:" << exists(str_handler.get_attr1(), forbidden_files_list) << std::endl;
				if(user.is_admin() || (user.is_admin() == false && exists(str_handler.get_attr1(), forbidden_files_list) == false)){

					if(str_handler.get_command_code() == 4){  // -f

						// TODO: not existed or not accept.
						sprintf(buf, "cd %s && rm -rf %s", pwd.c_str(), str_handler.get_attr1().c_str());
						exec(buf);
						sprintf(buf, "250: %s deleted.\n", str_handler.get_attr1().c_str());
						std::string log_str = "Ask for deleting file of " + str_handler.get_attr1();
						log(log_str);
					}
					
					else if(str_handler.get_command_code() == 7){ // rename
						// TODO: Check existance
						sprintf(buf, "cd %s && mv %s %s", pwd.c_str(), str_handler.get_attr1().c_str(), str_handler.get_attr2().c_str());
						exec(buf);
						sprintf(buf, "250: Successful change.\n");
						std::string log_str = "Rename " + str_handler.get_attr1() +  " to " + str_handler.get_attr2();
						log(log_str);
					}
					else if(str_handler.get_command_code() == 8){ // retr
						std::cout << "retr" << std::endl;

						std::string log_str = "Download " + str_handler.get_attr1();
						log(log_str);
					}
				}
				else{
					sprintf(buf, "550: File unavailable.\n");
					log("Ask for unavailable file.");
				}
			}
		}
	}
	
	send(command_socket, &buf, strlen(buf), 0);
}


int Client::get_status(){
	return status;
}