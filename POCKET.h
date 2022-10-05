#pragma once

struct Pocket
{
	enum class Command
	{
		Register,
		Login,
		Msg,
		Send_All,
		Ok,
		Error
	};

	Pocket() {};

	Pocket(Command&& com, const std::string& msg_s,
		const std::string& login_s, const std::string& password_s) : command(com)
	{
		strcpy(msg, msg_s.c_str());
		strcpy(login, login_s.c_str());
		strcpy(password, password_s.c_str());
	};


	Command command;
	char msg[256];
	char login[32];
	char password[32];

	friend std::string& operator+= (std::string& str, const Pocket& pock);
};

std::string& operator+= (std::string& str, const Pocket& pock)
{

	return str;
}