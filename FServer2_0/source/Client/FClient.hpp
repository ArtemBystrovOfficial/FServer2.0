
#include <asio.hpp>

#include "../Server/IO/buffer.hpp"
#include "../Server/IO/io.hpp"
#include "Servise/basic.hpp"
#include "Servise/filter_io.hpp"

#include <fstream>

/*
#define ASIO_STANDALONE 
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS
*/
using namespace asio;

template<class _Pocket>
using rfilter_ptr = std::shared_ptr < ReciverSingle<_Pocket> >;
template<class _Pocket>
using sfilter_ptr = std::shared_ptr < SenderSingle<_Pocket> >;

// Patern Facade of all project
template < class _Pocket >
class FClient
{
public:
	///////////////////
	//  Constructors
	//////////////////

	FClient() = delete;

	FClient(const std::string& ip, int port) :
		w_sender(new std::condition_variable),
		w_reciver(new std::condition_variable),
		io(new io_service)
	{
		buffer = std::make_shared<BufferIO<_Pocket>>(w_sender, w_reciver);
		basic = std::make_shared<BasicFClient<_Pocket>>(buffer, io, w_sender, ip, port);
		r_filter =  std::make_shared<ReciverSingle<_Pocket>>(buffer, w_reciver, basic);
		s_filter =  std::make_shared<SenderSingle<_Pocket>>(buffer, basic);
	}
	~FClient() {

		basic->setExit();
		w_reciver->notify_one();

	}

	/////////////////////////
	// connect and disconnect
	/////////////////////////

				//return false if bad ip or already connect
			bool connect();
				//return false if not connected
			bool disconect();

	//////////////////
	//	operators
	/////////////////

				// new very easy variant to send your pockets in one stroke with a lot of options, read documentations
			template < class _Pocket >
			friend FClient<_Pocket>& operator << (FClient<_Pocket>& fserver, const _Pocket &);
				// new very easy variant to send your pockets in one stroke with a lot of options, read documentations
			template < class _Pocket >
			friend FClient<_Pocket>& operator >> (FClient<_Pocket>& fserver, std::pair <_Pocket, typename ReciverSingle<_Pocket>::f_error> &);

	/////////////////
	// client status
	////////////////
	
				// return false if client not connected
			bool isConnected();
				// return true if your ip is banned
			bool isBanned();

	//////////////////
	// client health
	//////////////////		

				// if disconnect unsafety, you must refresh by hands
			bool refreshConnection();


private:

	////////////////////////////////////////
	// don't edit if you not developer this
	///////////////////////////////////////

	cv_ptr w_sender, w_reciver;

	bufferIO_ptr<_Pocket> buffer;

	service_ptr io;

	basic_ptr<_Pocket> basic;

	rfilter_ptr <_Pocket> r_filter;

	sfilter_ptr <_Pocket> s_filter;

};


template < class _Pocket >
bool FClient<_Pocket>::connect()
{
	return basic->connect();
}

template < class _Pocket >
bool FClient<_Pocket>::disconect()
{
	return basic->disconnect();
}

template < class _Pocket >
bool FClient<_Pocket>::isConnected()
{
	return basic->isConnected();
}

template < class _Pocket >
bool FClient<_Pocket>::isBanned()
{
	return basic->isBanned();
}

template < class _Pocket >
bool FClient<_Pocket>::refreshConnection()
{
	return basic->startHealthClient();
}

template < class _Pocket >
FClient<_Pocket>& operator << (FClient<_Pocket>& fserver, const _Pocket& pocket)
{
	fserver.s_filter->send_to(pocket);
	return fserver;
}

template < class _Pocket >
FClient<_Pocket>& operator >> (FClient<_Pocket>& fserver, std::pair <_Pocket, typename ReciverSingle<_Pocket>::f_error> & pocket)
{
	pocket = fserver.r_filter->recv();
	return fserver;
}