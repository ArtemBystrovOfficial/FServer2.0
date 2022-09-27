
// template class reliazation in header
#pragma once

#include <deque>
#include <mutex>
#include <condition_variable>
#include "../Servise/pocket.hpp"

typedef std::shared_ptr<std::condition_variable> cv_ptr;

template < class _Pocket >
class BufferIO
{
public:

	// now for debug only default
	BufferIO() = delete;

	//must use this
	BufferIO(cv_ptr cv_sender,cv_ptr cv_recvfilter)
	{
		waitgetOut = cv_sender;
		waitgetIn = cv_recvfilter;
	}

	//typedef _Pocket value_type;

	// return -1 Error and 0 when buff empty
	int getFIn(Pocket_Sys<_Pocket>&);

	// return -1 Error
	int addIn(const Pocket_Sys<_Pocket>&);

	// return -1 Error and 0 when buff empty
	int getFOut(Pocket_Sys<_Pocket >&);

	// return -1 Error
	int addOut(const Pocket_Sys<_Pocket>&);

	// none safety for thread be careful
	bool outIsEmpty()
	{
		return _out.empty();
	}

	bool inIsEmpty()
	{
		return _in.empty();
	}

	~BufferIO()
	{

	}

private:

	std::deque < Pocket_Sys<_Pocket> > _in , _out;

	std::mutex _lockIn , _lockOut;

	//for sender connect
	cv_ptr waitgetOut, waitgetIn;

};

template<class _Pocket>
int BufferIO<_Pocket>::getFIn(Pocket_Sys<_Pocket>& pocket)
{
	_lockIn.lock();

	if (_in.empty())
	{
		_lockIn.unlock();
		return 0;
	}

	pocket = _in.front();

	_in.pop_front();

	_lockIn.unlock();

	return 1;
}

template < class _Pocket>
inline int BufferIO<_Pocket>::addIn(const Pocket_Sys<_Pocket>& pocket)
{
	
	_lockIn.lock();

	_in.push_back(pocket);

	waitgetIn->notify_one();

	_lockIn.unlock();

	return 1;
}

template < class _Pocket>
inline int BufferIO<_Pocket>::getFOut(Pocket_Sys<_Pocket>& pocket)
{
	_lockOut.lock();

	if (_out.empty())
	{
		_lockOut.unlock();
		return 0;
	}

	pocket = _out.front();

	_out.pop_front();

	_lockOut.unlock();

	return 1;
}

template < class _Pocket>
inline int BufferIO<_Pocket>::addOut(const Pocket_Sys<_Pocket>& pocket)
{

	_lockOut.lock();

	_out.push_back(pocket);

	waitgetOut->notify_one();

	_lockOut.unlock();

	return 1;
}
