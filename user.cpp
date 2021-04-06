#include "user.h"

User::User(std::string username, std::string password, bool admin, int size): username(username), password(password), admin(admin), size(size) {}

std::string User::get_username(){
	return username;
}

std::string User::get_password(){
	return password;
}

bool User::is_admin(){
	return admin;
}

int User::get_size(){
	return size;
}