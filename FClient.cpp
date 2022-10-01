//#include "Client.hpp"
#include "FClient.hpp"
#include "POCKET.h"
#include <iostream>


using fec = ReciverSingle<Pocket>::f_error;

int main(int argc ,char ** argv)
{
	
	FClient <Pocket> cl("188.168.25.28", 21112);

	cl.connect();

	int mode = 0;

	std::pair<Pocket,fec> pocket;

	std::string msg, login, password;

	while (cl.isConnected())
	{
		std::cout << "0 - for Reg, 1 - for Login" << std::endl;
		std::cin >> mode;
		std::cout << "Login:\n";
		std::cin >> login;
		std::cout << "Password:\n";
		std::cin >> password;


		if (login.size() > 254 || password.size() > 254)
			throw std::exception("Login or pasword so long");

		if (mode)
		{//Login

			// Server -> client client -> Server
			cl << Pocket(Pocket::Command::Login, "", login, password);
			cl >> pocket;

			switch (pocket.first.command)
			{
			case(Pocket::Command::Error):
			{
				std::cout << "Error: " << pocket.first.msg << std::endl;
			} break;
			case(Pocket::Command::Ok):
			{
				std::cout << "Accept. Welcome\n";
				Sleep(1000);
				system("cls");

				std::thread send([&]() {

					Pocket pocket_in;

					while (cl.isConnected())
					{
						cl >> pocket;

						if (pocket.second == fec::ban)
						{
							std::cout << "your are banned\n";
							cl.disconect();
							exit(0);
						}

						if (pocket.second == fec::pause)
						{
							std::cout << "Server restart wait\n";
							continue;
						}

						switch (pocket.first.command)
						{
						case(Pocket::Command::Msg):
						{
							if (cl.isConnected())
								std::cout << pocket.first.msg << std::endl;
						}
						}
					}
					});

				while (cl.isConnected())
				{
					std::string msg;

					std::cin >> msg;

					if (msg == "$Exit")
					{
						cl.disconect();
						exit(0);
					}

					Pocket data(Pocket::Command::Send_All, msg, "", "");

					cl << data;
				}
				if(send.joinable())
					send.join();
			} break;
			}
		}
		else
		{//Registr

			// Server -> client client -> Server
			cl << Pocket(Pocket::Command::Register, "", login, password);
			cl >> pocket;

			switch (pocket.first.command)
			{
				case(Pocket::Command::Error):
				{
					std::cout << "Error: " << pocket.first.msg << std::endl;
				} break;
				case(Pocket::Command::Ok):
				{
					std::cout << "Registered" << std::endl;
				} break;
			}
		}
	}

	
	cl.disconect();
	

    return 0;
}
