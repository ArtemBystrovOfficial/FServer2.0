
#pragma once

#include "IO/buffer.hpp"
#include "IO/io.hpp"
#include "Servise/basic.hpp"
#include "Servise/filter_io.hpp"

//#include <asio.hpp>
/*
#define ASIO_STANDALONE 

#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS
*/

//using namespace asio;

//typedef std::shared_ptr<ip::tcp::socket> socket_ptr;

namespace fclient
{
	template<class _Pocket>
	using rfilter_ptr = std::shared_ptr < ReciverFilter<_Pocket> >;
	template<class _Pocket>
	using sfilter_ptr = std::shared_ptr < SenderFilter<_Pocket> >;
}

enum FType
{
		FID,
		GID,
		ALL
};

template < class _Pocket >
struct _Out
{

	_Pocket & pocket;

	FType type;
	
	int num;
};


// Patern Facade of all project
template < class _Pocket >
class FServer
{
public:
	///////////////////
	//  Constructors
	//////////////////

	FServer() = delete;

	FServer(const std::string & ip, int port):
		w_sender(new std::condition_variable),
		w_reciver(new std::condition_variable),
		io(new io_service)
	{
		buffer = std::make_shared<BufferIO<_Pocket>>(w_sender, w_reciver);
		basic = std::make_shared<BasicFServer<_Pocket>>(buffer, io, w_sender, ip, port);
		r_filter =  std::make_shared<ReciverFilter<_Pocket>>(buffer, w_reciver, basic);
		s_filter =  std::make_shared<SenderFilter<_Pocket>>(buffer, basic);
	}
	~FServer() {
	
		basic->setExit();
		w_reciver->notify_one();
	
	}
	/////////////////////////
	// start and stop listen
	/////////////////////////
				
				  // start your server, now all clients can connect to you
				void start();
				  // stop accept new connections, but other resume do it
				  // delete class for stop all process all exit from programm
				void stop();

	//////////////////
	//	operators
	/////////////////
	
	    // new very easy variant to send your pockets in one stroke with a lot of options, read documentations
	template < class _Pocket >
	friend FServer<_Pocket>& operator << (FServer<_Pocket>& fserver, const _Out<_Pocket>& out);
		// new very easy variant to send your pockets in one stroke with a lot of options, read documentations
	template < class _Pocket >
	friend FServer<_Pocket>& operator >> (FServer<_Pocket>& fserver, std::pair<_Pocket,int> & in);

	/////////////////
	//	groups	
	/////////////////
				   // return descriptor of gid, use him to next operations 
				 int newGroup(); 
				   // return descriptor of gid, use him to next operations 
				   // you can make group already with some fids
				 int newGroup(const std::vector<int> & vec_fids);
				   // return false if group doesn't exist or fid exist in group
				bool addToGroup(int gid, int fid);
				   // return false if group doesn't exist or fid exist in group
				bool deleteFromGroup(int gid, int fid);
				   // find all groups by fid
	std::vector<int> findGroupsByFid(int fid);
				   // return list of fids
	std::vector<int> getGroupList(int gid);
				   // return all gids
	std::vector<int> getGroups();

	/////////////////
	// server status
	////////////////
				   // is server listening
				bool isWorking();
				   // is current fid online
				bool isFidOnline(int fid);
				   // list of online users
	std::vector<int> getOnlineList();
				   // return last fid
				int  getMaxFid();
				   // return list Ip of online users
	std::vector < std::pair  < int,std::string > >	
				     getOnlineIpList();
				   // return list of Banned Ip fid -> string(IP)
std::set<std::string>getBanListIp();
				   

	/////////////////			
	// server health
	/////////////////
				   // refresh all sockets who closed not by safety exit ( Recomended to use sometimes )
				void refreshOnline();
				   // reserve to next versions
				void setOptions(int opt);
				   // reserve to next versions
				bool isServerCrash();

	//////////////////
	//server commands
	//////////////////
					// safety disconnect any fid
				bool disconnect(int fid);
					// ban by ip any fid
				void banIp(const std::string & ip);
					// unban by ip any fid
				bool unbanIp(const std::string& ip);
					// return ip by fid if fid not online return ""
				std::string getIpByFid(int fid);
				
private:

	////////////////////////////////////////
	// don't edit if you not developer this
	///////////////////////////////////////



	cv_ptr w_sender, w_reciver;

	bufferIO_ptr<_Pocket> buffer;

	service_ptr io;

	rf::basic_ptr<_Pocket> basic;

	fclient::rfilter_ptr <_Pocket> r_filter;

	fclient::sfilter_ptr <_Pocket> s_filter;


};

template < class _Pocket >
void FServer<_Pocket>::start()
{
	basic->Listen();
}

template < class _Pocket >
void  FServer<_Pocket>::stop()
{
	if(basic->isWorking())
		basic->_Off();
}

//////////////////
//	operators
/////////////////
template < class _Pocket >
FServer<_Pocket> & operator << (FServer<_Pocket>& fserver, const _Out<_Pocket>& out)
{
	switch (out.type)
	{
		case FType::FID:
		{
			fserver.s_filter->send_to(out.pocket,out.num);
		}
		break;
		case FType::GID:
		{
			fserver.s_filter->send_to_group(out.pocket, out.num);
		}
		break;
		case FType::ALL:
		{
			fserver.s_filter->send_to_all(out.pocket, out.num);
		}
		break;
	}
	return fserver;
}

template < class _Pocket >
FServer<_Pocket>& operator >> (FServer<_Pocket>& fserver, std::pair<_Pocket, int> & in)
{
	auto [pocket,fid] =  fserver.r_filter->recv();
	in.first = pocket;
	in.second = fid;

	return fserver;
}

/////////////////
//	groups	
/////////////////

template < class _Pocket >
int  FServer<_Pocket>::newGroup()
{
	return s_filter->new_group();
}

template < class _Pocket >
int  FServer<_Pocket>::newGroup(const std::vector<int> & vec_fids)
{
	return s_filter->new_group(vec_fids);
}

template < class _Pocket >
bool  FServer<_Pocket>::addToGroup(int gid, int fid)
{
	return s_filter->add_to_group(gid, fid);
}
template < class _Pocket >
bool  FServer<_Pocket>::deleteFromGroup(int gid, int fid)
{
	return s_filter->delete_from_group(gid, fid);
}

template < class _Pocket >
std::vector<int>  FServer<_Pocket>::findGroupsByFid(int fid)
{
	return s_filter->find_all_groups_by_fid(fid);
}

template < class _Pocket >
std::vector<int>  FServer<_Pocket>::getGroupList(int gid)
{
	return s_filter->get_group_list(gid);
}

template < class _Pocket >
std::vector<int>  FServer<_Pocket>::getGroups()
{
	return s_filter->get_groups();
}

/////////////////
// server status
////////////////

template < class _Pocket >
bool  FServer<_Pocket>::isWorking()
{
	return basic->isWorking();
}

template < class _Pocket >
bool  FServer<_Pocket>::isFidOnline(int fid)
{
	return basic->IsFidOnline(fid);
}

template < class _Pocket >
std::vector<int>  FServer<_Pocket>::getOnlineList()
{
	return basic->GetOnlineFidList();
}

template < class _Pocket >
int   FServer<_Pocket>::getMaxFid()
{
	return basic->get_current_fid();
}

/////////////////			
// server health
/////////////////

template < class _Pocket >
void  FServer<_Pocket>::refreshOnline()
{
	basic->startHealthServer();
}

template < class _Pocket >
void  FServer<_Pocket>::setOptions(int opt)
{
	// reserved
}

template < class _Pocket >
bool  FServer<_Pocket>::isServerCrash()
{
	// reserved
	return false;
}

//////////////////
//server commands
//////////////////

template < class _Pocket >
bool  FServer<_Pocket>::disconnect(int fid)
{

	if (!basic->IsFidOnline(fid))
		return false;

	return basic->Disconnect(fid);
}

template < class _Pocket >
void  FServer<_Pocket>::banIp(const std::string& ip)
{
	basic->_Ban(ip);
}

template < class _Pocket >
bool  FServer<_Pocket>::unbanIp(const std::string& ip)
{
	return basic->_UnBan(ip);
}

template < class _Pocket >
std::set<std::string> FServer<_Pocket>::getBanListIp()
{
	return basic->GetListBan();
}

template < class _Pocket >
std::vector < std::pair  < int, std::string > >
FServer<_Pocket>::getOnlineIpList()
{
	return basic->listIpOnline();
}

template < class _Pocket >
std::string FServer<_Pocket>::getIpByFid(int fid)
{
	return basic->getIpByFid(fid);
}