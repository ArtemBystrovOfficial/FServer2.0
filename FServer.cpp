//#include "Client.hpp"
#include "FServer.hpp"
#include "POCKET.h"
#include <Windows.h>

#include <iostream>


using Out = _Out<Pocket>;

int main(int argc ,char ** argv)
{
	
	FServer <Pocket> sv("192.168.0.200", 21112);

	sv.start();

	std::pair <Pocket, int> data;

	std::map  < std::string, std::string > data_u;

	std::map  <int, bool> is_login_u;

	auto online_group = sv.newGroup();

	while (sv.isWorking())
	{
		sv >> data;

		auto [pocket, fid] = data;

		switch (pocket.command)
		{
			case(Pocket::Command::Register):
			{
				auto is_be = data_u.find(pocket.login);
				if (is_be == data_u.end())
				{
					data_u[pocket.login] = pocket.password;
					sv << Out{ Pocket(Pocket::Command::Ok,"","",""),FType::FID, fid };
					break;
				}

				sv << Out{ Pocket(Pocket::Command::Error,"Login exsist","",""),FType::FID, fid };

			} break;
			case(Pocket::Command::Login):
			{
				auto password = data_u.find(pocket.login);
				if (password != data_u.end())
				{
					if (password->second == pocket.password)
					{
						is_login_u[fid] = true;
						sv.addToGroup(online_group, fid);
						sv << Out{ Pocket(Pocket::Command::Ok,"","",""),FType::FID, fid };
						break;
					}
				}

				sv << Out{ Pocket{Pocket::Command::Error,"uncorrect login or password","",""},FType::FID, fid };

			} break;
			case(Pocket::Command::Send_All):
			{
				auto connect = is_login_u.find(fid);
				if (connect != is_login_u.end())
				{
					if (connect->second)
					{
						sv << Out{ Pocket(Pocket::Command::Msg,pocket.msg,"",""),FType::ALL, fid };

						break;
					}
				}

				sv << Out{ Pocket{Pocket::Command::Error,"You don't auntification","",""},FType::FID, fid };

			} break;
		}
	}

	sv.stop();

    return 0;
}
