#include "str.h"

std::string Str_Handler::get_command(){
	return command;
}

std::string Str_Handler::get_attr1(){
	return attr1;
}

std::string Str_Handler::get_attr2(){
	return attr2;
}

int Str_Handler::get_num_attr(){
	return num_attr;
}

int Str_Handler::get_command_code(){
	return command_code;
}
int Str_Handler::get_size(){
	return size;
}

int Str_Handler::run(char *in , int input_size = 0){
	command_code = -1;
	num_attr = 0;

	size = input_size;
	std::string str(in);
	command = str;
	
	find_words();

	find_command();
	
	if(command_code != -1)
		find_attrs();
	return 0;
}

void Str_Handler::find_words(){
	int char1 = -1 , char2 = -1 , i = 0;
	for(i = 0 ; i < command.size() ; i ++){
		if(command[i] != ' '){
			char1 = i;
			char2 = find_end_word(i);
			words.push_back(command.substr(char1 , char2 - char1));
			
			i = char2;
		}
	}

//	for(i = 0; i < words.size() ; i++)
//		std::cout << words[i] << ".." << std::endl;
}

int Str_Handler::find_end_word(int start){
	int i = 0;
	for (i = start ; i < command.size() ; i++)
		if (command[i] == ' ')
			return i;

	return command.size();

}


bool Str_Handler::check_command(int i){
	std::string first = words[0];
	std::string candid = built_ins[i];

	int j = 0 , s = first.size();
	
	if (s != candid.size())
		return false;

	for(j = 0 ; j < s ; j ++)
	{
		if(first[j] != candid[j])
			return false;
	}
	return true;
}

void Str_Handler::find_command(){
	if (words.size() == 0)
		return ;
	int i = 0;
	for (i = 0 ; i < NUM_COMMAND ; i ++)
		if (check_command(i))
			command_code = i;

}

void Str_Handler::check_no_atrs(){
	if (words.size() != 1)
		command_code = -1;
}

void Str_Handler::check_one_atr(){
	if (words.size() > 2)
		return ;
	if(words.size() == 2){
		attr1 = words[1];
		num_attr++;
	}
}

void Str_Handler::check_two_atrs(){
	if (words.size() != 3)
		return;

	if(command_code == 4)
	{
		if(words[1] == "-f")
			command_code = 4;
		else if(words[1] == "-d")
			command_code = 11;
		else{
			command_code = -1;
			return ;
		}
		attr1 = words[2];
		num_attr ++;
	}

	else if(command_code == 7)
	{
		attr1 = words[1];
		attr2 = words[2];
		num_attr += 2;
	}
}

void Str_Handler::find_attrs(){
	
	if (command_code == 2 || command_code == 5 || command_code == 9 ||
		command_code == 10)
		check_no_atrs();

	else if(command_code == 0 || command_code == 1 || command_code == 3 || 
			command_code == 6 || command_code == 8)
		check_one_atr();

	else if(command_code == 4 || command_code == 7)
		check_two_atrs();
}