#pragma once
template <typename _Pocket>
struct Pocket_Sys
{
	_Pocket pocket;

	// Servise varibles
	int fid; // fid of client

	// not fot use at down

	uint64_t _pocket_id; // number of client pocket for
						 // find loses pockets end wait then
	int extra_test = 777;

	bool is_command = false;

	enum class commands
	{
		CloseMe,
		IsBan,
		IsOff
	};

	commands command;

	//extra test for disconnect
	bool is_active = false;

	template <typename _Pocket>
	friend bool operator<(const Pocket_Sys& pock1, const Pocket_Sys& pock2);
};

template <typename _Pocket>
bool operator<(const Pocket_Sys<_Pocket> & pock1, const Pocket_Sys<_Pocket> & pock2)
{
	return pock1._pocket_id < pock2._pocket_id;
}