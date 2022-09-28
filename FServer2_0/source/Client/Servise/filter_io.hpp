#pragma once

#include "basic.hpp"

#include "../../Server/Servise/pocket.hpp"

#include <algorithm>

template <typename _Pocket>
using basic_ptr = std::shared_ptr < BasicFClient <_Pocket > >;

#define FOREVER for(;;)

template <class _Pocket>
class ReciverSingle
{

public:

	ReciverSingle() = delete;

	ReciverSingle(bufferIO_ptr<_Pocket> ptr, cv_ptr  waitget, basic_ptr <_Pocket> basic_server)
	{
		this->waitget = waitget;
		this->basic_server = basic_server;
		buffer_io = ptr;
	}

	///////////////////////////////////////////////
	// Filter for server calls and users pockets // 
	///////////////////////////////////////////////

	_Pocket recv();

private:

	basic_ptr <_Pocket> basic_server;
	cv_ptr waitget;
	std::mutex mtx;
	bufferIO_ptr <_Pocket> buffer_io;

};

template <class _Pocket>
_Pocket ReciverSingle<_Pocket>::recv()
{

	FOREVER
	{
		{
			std::unique_lock <std::mutex> lock(mtx);
			waitget->wait(lock, [&] { return !buffer_io->inIsEmpty() || basic_server->isExit();  });
		}

		if (basic_server->isExit())
			break;



		Pocket_Sys <_Pocket> pocket;
		if (buffer_io->getFIn(pocket) != 0)
		{
			if (pocket.fid == basic_server->getConnectSession())
			{
				if (pocket.is_command)
				{
					// Systems branches
					///////////////////////////
					/// You can configure Pocket_Sys 
					/// -> enum commands <-
					/// if you want add your functions
					/// without use this in main Pocket
					/// use it this, it's power tool
					//////////////////////////

					/// Reserve for next updates


					switch (pocket.command)
					{
					case Pocket_Sys<_Pocket>::commands::CloseMe:
					{

						//std::cout << "TESTING CLOSEME: " << pocket.fid << std::endl;
						// Reserve for next use

					}
					break;

					default:
					{

					}
					break;
					}
				}
				else
				{
					// Pockets to user
					return pocket.pocket;

				}
			}
		}
	}
}

template <class _Pocket>
class SenderSingle
{
public:

	SenderSingle(bufferIO_ptr<_Pocket> buffer_io_ptr, basic_ptr<_Pocket> basic_ptr) :
		buffer_io(buffer_io_ptr),
		basic_fclient(basic_ptr)
	{}

	bool send_to(const _Pocket&);

private:
	bufferIO_ptr<_Pocket> buffer_io;
	basic_ptr<_Pocket> basic_fclient;
};

template <class _Pocket>
bool SenderSingle<_Pocket>::send_to(const _Pocket& pocket)
{

	basic_fclient->startHealthClient();

	if (!basic_fclient->isConnected())
		return false;

	Pocket_Sys<_Pocket> _pocket;

	_pocket.pocket = pocket;

	_pocket.fid = basic_fclient->getConnectSession();

	_pocket.is_command = false;

	buffer_io->addOut(_pocket);

	return true;
}

