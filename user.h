#ifndef USER_H
#define USER_H

#include <string>

class User{
public:
	User(std::string username, std::string password, bool admin, int size);

	std::string get_username();
	std::string get_password();
	bool is_admin();	
	int get_size();

private:
	std::string username, password;
	bool admin;
	int size;
};


#endif