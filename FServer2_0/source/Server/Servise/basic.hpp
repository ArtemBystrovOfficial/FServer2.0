#pragma once

#include <asio.hpp>
#include <atomic>
#include <memory>
#include <map>
#include <utility>


#include "../IO/buffer.hpp"
#include "../IO/io.hpp"

#include "pocket.hpp"
#include <string>

using namespace asio;
typedef std::shared_ptr<ip::tcp::socket> socket_ptr;
typedef std::shared_ptr<std::atomic <bool> > bool_atc_ptr;

template <typename _Pocket>
using bufferIO_ptr = std::shared_ptr< BufferIO <_Pocket > >;
typedef std::shared_ptr<io_service> service_ptr;

typedef std::shared_ptr<ip::tcp::acceptor> accepter_ptr;

////////////////////////////////////
// Center of server, main purpose
// journal all connections, show online
// clean exit clients
////////////////////////////////////

template<class _Pocket>
struct User_Session
{
	User_Session() = delete;

	User_Session(
		bool_atc_ptr bool_ptr,
		socket_ptr sock_ptr,
		bufferIO_ptr <_Pocket> buffer_io
	) : reciver(sock_ptr,bool_ptr,buffer_io),
		socket(sock_ptr),
		socket_closed(bool_ptr)
	{}

	bool_atc_ptr socket_closed{ new std::atomic <bool>{ false } };
	socket_ptr socket;
	Reciver<_Pocket> reciver;
};

template<class _Pocket>
class BasicFServer
{
	//extern friend class FServer<_Pocket>;
public:

	BasicFServer() = delete;
	BasicFServer(bufferIO_ptr<_Pocket> ptr
		, service_ptr service
		, const std::string& ip
		, int port)
		: ep(ip::address::from_string(ip), port)
		, io_s(service)
		, buffer_io(ptr)
	{
		sender.start(buffer_io,cv_ptr(new std::condition_variable));
	}

	// Listening new connections
	void Listen();

	// Disconnect current fid
	// -1 if fid don't exist 0 deleted
	int Disconnect(int fid);

	// Send Pocket_Sys to all with refresh
	void Refresh();

	// Send Pocket_Sys to all with refresh
	std::vector <int> GetOnlineFidList();

	// Send Pocket_Sys to all with refresh
	bool IsFidOnline(int fid);

	// Servise commands from clients
	void PortNewSystemPocket(Pocket_Sys<_Pocket> what);

	// Listening or not
	bool isWorking();

	// {1...inf} some problems, count of cleaning pockets -> all okay
	int startHealthServer();

	// Safety stop listening
	void _Off();

	~BasicFServer();

	// don't use socket->close() use this function for safe
	static int _close_socket(socket_ptr, bool_atc_ptr);

private:

	//void _New_Connection()

	void _listenner();

private:

	// all fid start with 1

	int32_t current_free_fid = 1;

	// use before all connect with users locked-based tehnology
	std::mutex _block_map_users;

	service_ptr io_s;

	std::thread _listen_run;

	ip::tcp::endpoint ep;
	//{ ip::address::from_string(ip), port };

	accepter_ptr acceptor;
	//(*service, ep);

	bufferIO_ptr<_Pocket> buffer_io;

	std::atomic <bool> is_working;

	// id < Socket, Reciver >
	std::map < int32_t, User_Session<_Pocket> > users;

	//Sender Part
	Sender <_Pocket> sender;

};

template<class _Pocket>
inline void BasicFServer<_Pocket>::Listen()
{
	if(!is_working.load())
	_listen_run = std::thread([&]() {
		_listenner();
	});
}

// thread function of connect listenner

template<class _Pocket>
void BasicFServer<_Pocket>::_listenner()
{

	static int is_first = 0;
	if (!is_first)
	{
		acceptor = std::shared_ptr<ip::tcp::acceptor>(new ip::tcp::acceptor(*io_s, ep));
	}

	is_working.store(true);

	while (is_working.load())
	{
		//Pre-settings
		error_code ec;

		socket_ptr sock(new ip::tcp::socket(*io_s));
		bool_atc_ptr check_connect_closed(new std::atomic<bool>);
		check_connect_closed->store(false);

		acceptor->accept(*sock,ec);

		if (!is_working.load())
					break;

///////////////////////////
// ADD BLOCK OF MANAGE USERS
//////////////////////////

		_block_map_users.lock();

		users.emplace(std::piecewise_construct,
			std::forward_as_tuple(current_free_fid),
			std::forward_as_tuple(check_connect_closed, sock, buffer_io));

		sender._addNewFidSocket(current_free_fid, sock);

		//users.emplace(check_connect_closed, User_Session<_Pocket>(check_connect_closed, sock, buffer_io));

		_block_map_users.unlock();

//////////////////////////
// PLEASE DON'T EDIT
//////////////////////////
			
		++current_free_fid;
	}

	is_working.store(false);
}


template<class _Pocket>
inline int BasicFServer<_Pocket>::Disconnect(int fid)
{
	_block_map_users.lock();

	auto it = users.find(fid);

	if (it == users.end())
	{
		_block_map_users.unlock();
		return -1;
	}

	auto& reciver = it->second.reciver;

	// Use always before exit
	_close_socket(it->second.socket, it->second.socket_closed);
	
	//Dont forget about _close_socket
	if (reciver.ext() == -1)
	{
		// error
		std::cout << " EXTRA ERROR DISCONECT BASICFSERVER USE _CLOSE_SOCKET\n";
		throw std::exception("bad ext");
	}

	//delete connect from map
	users.erase(it);

	_block_map_users.unlock();

	return 0;
}

template<class _Pocket>
inline void BasicFServer<_Pocket>::Refresh()
{
	// SENDERFILTER WAIT
}

template<class _Pocket>
inline std::vector<int> BasicFServer<_Pocket>::GetOnlineFidList()
{
	// optimazation return rule of 3

	int i = 0;

	_block_map_users.lock();

	std::vector<int> list(users.size());

	for (auto& it : users)
	{
		list[i] = it.first;
		i++;
	}

	_block_map_users.unlock();

	return list;
}

template<class _Pocket>
inline bool BasicFServer<_Pocket>::IsFidOnline(int fid)
{
	_block_map_users.lock();

	const auto it = users.find(fid);

	bool is = it == users.end() ? false : true;

	_block_map_users.unlock();

	return is;

}

template<class _Pocket>
inline void BasicFServer<_Pocket>::PortNewSystemPocket(Pocket_Sys<_Pocket> what)
{
	// SENDERFILTER WAIT
}

template<class _Pocket>
inline bool BasicFServer<_Pocket>::isWorking()
{
	return is_working.load();
}

template<class _Pocket>
inline int BasicFServer<_Pocket>::startHealthServer()
{

	int count_errors = 0;

	_block_map_users.lock();

	bool is = true;
	int for_delete;
	while (is)
	{
		is = false;
		for_delete = 0;

		for (auto& it : users)
		{
			if (!it.second.reciver.isWorking() || it.second.socket_closed->load())
			{
				count_errors++;

				for_delete = it.first;

				is = true;
				break;
			}
		}
		if (for_delete)
		{
			_block_map_users.unlock();

			Disconnect(for_delete);

			_block_map_users.lock();
		}
	}

	_block_map_users.unlock();

	return count_errors;
}

template<class _Pocket>
inline void BasicFServer<_Pocket>::_Off()
{
	is_working.store(false);

	acceptor->close();

	_listen_run.join();
}

template<class _Pocket>
inline BasicFServer<_Pocket>::~BasicFServer()
{

	sender.ext();

	_block_map_users.lock();

	for (auto & it : users)
	{
		auto & reciver = it.second.reciver;

		// Use always before exit
		_close_socket(it.second.socket, it.second.socket_closed);

		//Dont forget about _close_socket
		if (reciver.ext())
		{
			// error
			std::cout << " EXTRA ERROR DESTRUCTOR BASICFSERVER USE _CLOSE_SOCKET\n";
		}
	}


	_block_map_users.unlock();

	is_working.store(false);

	acceptor->close();

	_listen_run.join();

}

template<class _Pocket>
inline int BasicFServer<_Pocket> ::_close_socket(socket_ptr sock, bool_atc_ptr is)
{
	if (is->load())
		return -1;
	sock->close();

	is->store(true);
	return 0;
}