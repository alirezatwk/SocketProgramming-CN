#include "user.h"

User::User() {}

User::User(std::string username, std::string password, bool admin, int size): username(username), password(password), admin(admin), size(size) {}

User::User(const User &user): username(user.username), password(user.password), admin(user.admin), size(user.admin) {}

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

void User::set_username(std::string new_username){
	username = new_username;
}