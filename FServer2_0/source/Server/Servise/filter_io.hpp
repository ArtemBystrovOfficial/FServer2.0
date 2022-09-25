#pragma once

#include "../IO/buffer.hpp"

#include "basic.hpp"

#include "../Servise/pocket.hpp"

#include <algorithm>

template <typename _Pocket>
using basic_ptr = std::shared_ptr < BasicFServer <_Pocket > >;

#define FOREVER for(;;)

template <class _Pocket>
class ReciverFilter
{

public:

	ReciverFilter() = delete;

	ReciverFilter(bufferIO_ptr<_Pocket> ptr, cv_ptr  waitget, basic_ptr <_Pocket> basic_server)
	{
		this->waitget = waitget;
		buffer_io = ptr;
	}

	///////////////////////////////////////////////
	// Filter for server calls and users pockets // 
	///////////////////////////////////////////////

	std::pair <_Pocket, int> recv();

private:

	basic_ptr <_Pocket> basic_server;
	cv_ptr waitget;
	std::mutex mtx;
	bufferIO_ptr <_Pocket> buffer_io;

};

template <class _Pocket>
std::pair <_Pocket, int> ReciverFilter<_Pocket>::recv()
{

	FOREVER
	{
		{
			std::unique_lock <std::mutex> lock(mtx);
			waitget->wait(lock, [&] { return !buffer_io->inIsEmpty();  });
		}

		Pocket_Sys <_Pocket> pocket;
		if (buffer_io->getFIn(pocket) != 0)
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

					std::cout << "TESTING CLOSEME: " << pocket.fid << std::endl;
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

				return { pocket.pocket,pocket.fid };

			}
		}
	}
}

template <class _Pocket>
class SenderFilter
{
public:

	SenderFilter(bufferIO_ptr<_Pocket> buffer_io_ptr, basic_ptr<_Pocket> basic_ptr):
																buffer_io(buffer_io_ptr),
																basic_fserver(basic_ptr)
	{}
						

	//////////////////////////////
	// all functions return false
	// if fid or group didn't find
	//////////////////////////////

	bool send_to(const _Pocket &, int fid);

	// return how many messages sended
	// return -1 if didn't find
	int send_to_group(const _Pocket &, int gid);

	// return how many messages sended
	// fid_ignore -1 -> all -> fid_ignore 1 all wihout fid 1
	int send_to_all(const _Pocket &, int fid_igonre);

	bool add_to_group(int gid, int fid);
	bool delete_from_group(int gid, int fid);
	std::vector<int> find_all_groups_by_fid(int fid);

	//return gid
	int new_group();
	//return gid
	int new_group(std::vector<int> vec_fids);

	std::vector<int> get_group_list(int gid);
	std::vector<int> get_groups();

private:

	bufferIO_ptr<_Pocket> buffer_io;
	basic_ptr<_Pocket> basic_fserver;
	std::map< int , std::vector <int> > groups;

};

template<class _Pocket>
inline bool SenderFilter<_Pocket>::send_to(const _Pocket & pocket, int fid)
{
	basic_fserver->startHealthServer();

	if (!basic_fserver->IsFidOnline(fid))
		return false;
	
	Pocket_Sys<_Pocket> _pocket;

	_pocket.pocket = pocket;

	_pocket.fid = fid;

	buffer_io->addOut(_pocket);

	return true;
}

template<class _Pocket>
inline int SenderFilter<_Pocket>::send_to_group(const _Pocket& pocket, int gid)
{
	auto it = groups.find(gid);
	if (it == groups.end())
		return -1;

	Pocket_Sys<_Pocket> _pocket;

	_pocket.pocket = pocket;

	int count_out = 0;

	basic_fserver->startHealthServer();

	for (auto & fid : it->second)
	{
		if (basic_fserver->IsFidOnline(fid))
		{
			_pocket.fid = fid;
			buffer_io->addOut(_pocket);
			count_out++;
		}
	}
	
	return count_out;
}

template<class _Pocket>
inline int SenderFilter<_Pocket>::send_to_all(const _Pocket& pocket,int fid_ignore)
{
	Pocket_Sys<_Pocket> _pocket;

	_pocket.pocket = pocket;

	int count_out = 0;

	basic_fserver->startHealthServer();

	auto list = basic_fserver->GetOnlineFidList();

	for (auto & fid : list)
	{
		if (fid_ignore != fid)
		{
			_pocket.fid = fid;
			buffer_io->addOut(_pocket);
			count_out++;
		}
		
	}

	return count_out;
}

template<class _Pocket>
inline bool SenderFilter<_Pocket>::add_to_group(int gid, int fid)
{
	auto it = groups.find(gid);
	if (it == groups.end())
		return false;

	auto & vec = it->second;

	auto it_vec = std::find(vec.begin(), vec.end(), fid);

	if (it_vec != vec.end())
		return false;

	vec.push_back(fid);

	return true;
}

template<class _Pocket>
inline bool SenderFilter<_Pocket>::delete_from_group(int gid, int fid)
{
	auto it = groups.find(gid);
	if (it == groups.end())
		return false;

	auto& vec = it->second;

	auto it_vec = std::find(vec.begin(), vec.end(), fid);

	if (it_vec == vec.end())
		return false;

	vec.erase(it_vec);

	return true;
}

template<class _Pocket>
inline std::vector<int> SenderFilter<_Pocket>::find_all_groups_by_fid(int fid)
{
	std::vector<int> vec_groups;

	for (auto & num : groups)
	{
		auto & vec = num.second;
		auto & it = std::find(vec.begin(), vec.end(), fid);

		if(it == vec.end())
				vec_groups.push_back(num.first);
			
	}

	return vec_groups;
}

template<class _Pocket>
inline int SenderFilter<_Pocket>::new_group()
{
	int gid = groups.size() + 1;
	groups[gid] = std::vector<int>{};
	return gid;
}

template<class _Pocket>
inline int SenderFilter<_Pocket>::new_group(std::vector<int> vec)
{
	int gid = groups.size() + 1;
	groups[gid] = vec;
	return gid;
}

template<class _Pocket>
inline std::vector<int> SenderFilter<_Pocket>::get_group_list(int gid)
{
	auto it = groups.find(gid);
	if (it == groups.end())
		return {};

	return it->second;

}

template<class _Pocket>
inline std::vector<int> SenderFilter<_Pocket>::get_groups()
{
	std::vector<int> v;

	for (int i = 1; i <= groups.size(); i++)
	{
		v.push_back(i);
	}
	return v;
}
