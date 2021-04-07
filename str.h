#ifndef STR_H
#define STR_H


#include <string>
#include <vector>
#include <iostream>

#define NUM_COMMAND 11

const std::string built_ins [] = {"user" , "pass" , "pwd" , "mkd" , "dele" , "ls" , "cwd" , "rename" , "retr" , "help" , "quit"}; 

class Str_Handler{
public:
	int get_num_attr();
	std::string get_attr1();
	std::string get_attr2();
	int get_size();
	std::string get_command();
	int get_command_code();
	
	void find_words();
	int find_end_word(int start);
	int run(char *in , int input_size);
	void find_command();
	bool check_command(int i);
	void find_attrs();
	void check_no_atrs();
	void check_one_atr();
	void check_two_atrs();
	

private:
	std::vector<std::string> words;
	std::string command , attr1 , attr2;
	int size;
	int num_attr , command_code;

	int checked;
};


#endif
