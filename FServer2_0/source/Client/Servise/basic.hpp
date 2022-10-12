#pragma once

#include <asio.hpp>

#include "../../Server/IO/buffer.hpp"
#include "../../Server/IO/io.hpp"

typedef std::shared_ptr<ip::tcp::socket> socket_ptr;
typedef std::shared_ptr<std::atomic <bool> > bool_atc_ptr;

template <typename _Pocket>
using bufferIO_ptr = std::shared_ptr< BufferIO <_Pocket > >;
typedef std::shared_ptr<io_service> service_ptr;

typedef std::shared_ptr<ip::tcp::acceptor> accepter_ptr;

template<class _Pocket>
class BasicFClient
{
public:
	BasicFClient() = delete;
	BasicFClient(bufferIO_ptr<_Pocket> ptr
		, service_ptr service
		, cv_ptr wait_sender
		, const std::string& ip
		, int port)
		: ep(ip::address::from_string(ip), port)
		, io_s(service)
		, buffer_io(ptr)
		, socket_closed(new std::atomic<bool>)
		, empty(new std::atomic<bool>)
		, non_information(new std::atomic<bool>(false))
		, is_connected(false)
	{
		is_exit.store(false);
		sender.setInformation(non_information);
		sender.start(buffer_io, wait_sender);
		socket_closed->store(true);
	}

	// if alreay connected or ip and port bad -> return false
	bool connect();
	// if not conected
	bool disconnect();
	// is connected
	bool isConnected();
	// return true if connection lost
	bool startHealthClient();

	bool isExit()
	{
		return is_exit.load();
	}

	void setExit()
	{
		is_exit.store(true);
	}

	// last fid
	int getConnectSession()
	{
		return _current_connection.load();
	}

	bool isBanned()
	{
		return is_banned.load();
	}

	void _SetBanned()
	{
		is_banned.store(true);
	}


	~BasicFClient();
private:
	
	socket_ptr server;
		
	service_ptr io_s;

	ip::tcp::endpoint ep;

	bufferIO_ptr<_Pocket> buffer_io;

	std::atomic <bool> is_exit;
	std::atomic <bool> is_connected;
	bool_atc_ptr socket_closed;

	//non anviliable there
	bool_atc_ptr empty;
	bool_atc_ptr non_information;

	std::atomic <int> _current_connection{0}; // fid
	std::atomic <bool> is_banned{false};

	Sender <_Pocket> sender;
	Reciver <_Pocket> * reciver;



};

template<class _Pocket>
inline bool BasicFClient<_Pocket>::connect()
{

	if (!is_connected.load())
	{
		error_code ec;
		server = std::make_shared<ip::tcp::socket>(*io_s);

		server->connect(ep,ec);

		if (ec)
		{
			return false;
		}

		socket_closed->store(false);

		_current_connection.store(_current_connection.load() + 1);

		is_connected.store(true);

		sender._addNewFidSocket(_current_connection.load(), server);
		
		reciver = new Reciver <_Pocket>(server,
			socket_closed,
			buffer_io,
			_current_connection.load(),
			empty		
			);

		reciver->setInformation(non_information);

		is_banned.store( false );

	}
	else
		return false;
	
	return true;
}

template<class _Pocket>
inline bool BasicFClient<_Pocket>::disconnect()
{
	if (is_connected.load())
	{
		if (reciver != nullptr)
		{
			server->close();
			reciver->ext();
			delete reciver;
		}

		is_connected.store(false);
	}
	else
		return false;

	return true;
}

template<class _Pocket>
inline bool BasicFClient<_Pocket>::isConnected()
{
	return is_connected.load();
}
template<class _Pocket>
inline BasicFClient<_Pocket>::~BasicFClient()
{
	disconnect();
}

template<class _Pocket>
inline bool BasicFClient<_Pocket>::startHealthClient()
{
	if (socket_closed->load())
	{
		return disconnect();
	}
	else
		return false;
}