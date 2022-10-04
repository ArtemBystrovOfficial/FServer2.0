#pragma once
#include <thread>
#include <atomic>
#include <asio.hpp>
#include <algorithm>
#include <map>
#include <condition_variable>


const uint64_t max_count_pockets = 63'072'000'000; // 20 pocket / sec for 100 years

#include "Servise/pocket.hpp"
#include "buffer.hpp"

//#define TEST_CON1 // iostream enable for some tests
//#define TEST_CON2 // iostream enable for some tests

//#define DEBUG_SENDER

#ifdef TEST_CON1 
	#include <iostream>
#endif
#ifdef TEST_CON2
	#include <iostream>
#endif
using namespace asio;

typedef std::shared_ptr<ip::tcp::socket> socket_ptr;
typedef std::shared_ptr<std::atomic <bool> > bool_atc_ptr;
template <typename _Pocket>
using bufferIO_ptr = std::shared_ptr< BufferIO <_Pocket > >;
typedef std::shared_ptr<std::condition_variable> cv_ptr;

/////////////////
//Reciver Begin 
/////////////////

template<class _Pocket>
class Reciver
{
public:

	Reciver() = delete;

	// start Reciver when contruction
	Reciver(socket_ptr ptr,
	bool_atc_ptr socket_close_status,
	bufferIO_ptr<_Pocket>, int fid,
	bool_atc_ptr new_diconnect);

	//return 0 if socket close and recived is safety closed
	//return -1 if socket doesn't closed and close does'nt safery
	int ext();

	bool isWorking() const
	{
		return !is_ext.load();
	}

	//if you dont use ext() try make this for us, but close socket(Bad idea)
	~Reciver();

private:

	//external
	socket_ptr sock;
	bufferIO_ptr<_Pocket> buffer_io;

	void recive_from();

	// Manage lifetime reciver
	int fid;
	std::thread _run;
	std::atomic<bool> is_ext;
	bool_atc_ptr _is_socket_closed;
	bool_atc_ptr new_diconnect;

	// system of right pocket tcp connect
	std::vector<Pocket_Sys<_Pocket>> _pockets_queue;
	int _current_pocket_id;

};

template<class _Pocket>
Reciver<_Pocket>::Reciver(socket_ptr ptr, bool_atc_ptr socket_close_status, bufferIO_ptr<_Pocket> buffer_io,int fid, bool_atc_ptr new_diconnect) : sock(ptr),
																			  _is_socket_closed(socket_close_status),
																			  _current_pocket_id(1),
																			  buffer_io(buffer_io),
																			  fid(fid),
																			  new_diconnect(new_diconnect)
																			  
{
	if (socket_close_status.get() == nullptr)
		exit(0);
	_run = std::thread([&]() {

		recive_from();

	});
}

template<class _Pocket>
void Reciver<_Pocket>::recive_from()
{
	error_code ec;

	Pocket_Sys<_Pocket> pocket;

	while (!is_ext.load())
	{
		pocket.is_active = false;

		sock->read_some(asio::buffer(&pocket, sizeof(Pocket_Sys<_Pocket>)), ec);

		//if sock not connected now, its okay this parametr skip this and wait for that
		if (ec == error::bad_descriptor)
			continue;

		//If socket closed
		if (ec == error::eof || is_ext.load() || !pocket.is_active)
		{
			if (ec == error::eof || || !pocket.is_active)
			{
				new_diconnect->store(true);
				_is_socket_closed->store(true);
			}
			break;
		}

		// set fid for next manipulations
		pocket.fid = fid;

		if (!is_ext.load())
		{

			//extra safe all _pocked_id start with 1
			if(pocket._pocket_id < max_count_pockets && pocket._pocket_id > 0 && pocket.extra_test == 777 )
			{ 

/////////////////////////////////////////////////////
// system of wait pockets which comes not at his place
////////////////////////////////////////////////////

				if (pocket._pocket_id == _current_pocket_id)
				{
					++_current_pocket_id;
#ifdef TEST_CON2
					std::cout << "number of pocket: " << pocket._pocket_id << "fid: " << pocket.fid << "\n";
#endif
					// add pocket to bufferIO
					buffer_io->addIn(pocket);

					if (!_pockets_queue.empty())
					{
						std::sort(_pockets_queue.begin(), _pockets_queue.end());
						bool is_be=true;
						while (is_be)
						{
							is_be = false;
							for (auto it = _pockets_queue.begin(); it != _pockets_queue.end(); it++)
							{
								if (it->_pocket_id == _current_pocket_id)
								{
									is_be = true;
#ifdef TEST_CON2
									std::cout << "number of pocket: " << it->_pocket_id << "fid: " << it->fid << "\n";
#endif	
									// add pocket to bufferIO
									buffer_io->addIn(*it);

									_pockets_queue.erase(it);
									++_current_pocket_id;

									break;
								}
							}
						}
					}
				}
				else
				{
					_pockets_queue.push_back(pocket);
				}

/////////////////////////////////////////////////////
// please don't edit this if you don't know tehnology
////////////////////////////////////////////////////

#ifdef TEST_CON1
				std::cout << "OUT OF POCKET TEST\n";
				std::cout << "fid, pocket_id, pocket " << pocket.fid
												<< " " << pocket._pocket_id 
												<< " " << pocket.pocket <<"\n"; // use operator std::cout << pocket in tests
				std::cout << "------------------\n";
#endif		
			}
#ifdef TEST_CON1
				else
				std::cout << "BROKING POCKET\n";
#endif
		}
	}
	is_ext.store(true);
}

template<class _Pocket>
int Reciver<_Pocket>::ext()
{

	if (!_is_socket_closed->load())
			return -1;

	is_ext.store(true);

	if(_run.joinable())
		_run.join();

	return 0;
}

template<class _Pocket>
Reciver<_Pocket>::~Reciver()
{
	if (!is_ext.load())
	{
		sock->close();
		_is_socket_closed->store(true);
		is_ext.store(true);
	}

	if(_run.joinable())
		_run.join();	
}

/////////////////
//Reciver End
/////////////////


/////////////////
//Sender Begin 
/////////////////

template <class _Pocket>
class Sender
{
public:

	// start Sender when contruction
	Sender();

	//safety exit please use this
	void ext();

	bool isWorking() const
	{
		return !is_ext.load();
	}

	//if you dont use ext() try make this for us, but close socket(Bad idea)
	~Sender();


// Connect with memory of sockets -- fid //
	/////////////////////////////
	// BASICFSERVER(_LISTENING //
	//     |				   //
	// ADDNEWFID			   //
	/////////////////////////////
	//   All sockets copyble   //

	// if fid don't right return -1;

	int _addNewFidSocket(int fid, socket_ptr);

	void start(bufferIO_ptr<_Pocket>, cv_ptr wait_buf);

private:

	socket_ptr getSocketbyFid(int fid);

	//external
	bufferIO_ptr<_Pocket> buffer_io;
	void send_to();

	// Manage lifetime sender
	std::thread _run;
	std::atomic<bool> is_ext;

	// system of right pocket tcp connect
	std::vector <socket_ptr> sockets;
	std::map<int, int> last_pocket;
	std::mutex mtx;
	cv_ptr waitget;
	error_code ec;

};

template <class _Pocket>
int  Sender<_Pocket>::_addNewFidSocket(int fid, socket_ptr ptr)
{
	if (fid - 1 != sockets.size())
		return -1;
		
	mtx.lock();

	sockets.push_back(ptr);

	mtx.unlock();

	return 0;
}

template <class _Pocket>
socket_ptr Sender<_Pocket>::getSocketbyFid(int fid)
{
	if (fid > sockets.size())
		return nullptr;
	else
		return sockets[fid - 1];

}

template<class _Pocket>
Sender<_Pocket>::Sender()
{}

template<class _Pocket>
void Sender<_Pocket>::start(bufferIO_ptr<_Pocket> buffer_io, cv_ptr wait_buf)
{
	this->buffer_io = buffer_io;
	this->waitget = wait_buf;
	_run = std::thread([&]() {

		send_to();

	});
}

template<class _Pocket>
void Sender<_Pocket>::send_to()
{
	Pocket_Sys<_Pocket> pocket;
	
	while (!is_ext.load())
	{

		// select and poll
		// block_out 

		// wait for buffer
		{
			std::unique_lock<std::mutex> lock(mtx);
			waitget->wait(lock, [&] {		
				return !buffer_io->outIsEmpty() || is_ext.load();
			});
		}

		if ( buffer_io->getFOut(pocket) == 0)
		{
			continue;
		}


#ifndef DEBUG_SENDER	
		mtx.lock();

		auto sock = getSocketbyFid(pocket.fid);

		mtx.unlock();
		if (sock == nullptr)
		{
			//error sock-fid
			continue;
		}
#endif
		auto it = last_pocket.find(pocket.fid);

		int out;

		if (it == last_pocket.end())
		{
			out = 1;
			last_pocket[pocket.fid] = 1;
		}
		else
		{
			out = it->second;
			last_pocket[pocket.fid]++;
			out++;
		}

		pocket._pocket_id = out;


#ifndef DEBUG_SENDER

		pocket.is_active = true;

		sock->write_some(asio::buffer(&pocket, sizeof(Pocket_Sys<_Pocket>)),ec);
		if (ec)
			continue;
#else
		std::cout << pocket.pocket.n << " " << pocket.fid << " " << pocket._pocket_id << std::endl;
#endif

	}


	is_ext.store(true);
}

template<class _Pocket>
void Sender<_Pocket>::ext()
{

	is_ext.store(true);

	waitget->notify_one();
	if(_run.joinable())
		_run.join();
}

template<class _Pocket>
Sender<_Pocket>::~Sender()
{
	if (!is_ext.load())
	{
		ext();
	}
}


/////////////////
//Sender End
/////////////////

