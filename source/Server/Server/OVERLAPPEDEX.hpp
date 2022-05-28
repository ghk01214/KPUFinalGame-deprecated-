#ifndef _OVERLAPPEDEX_HPP_
#define _OVERLAPPEDEX_HPP_

#include "protocol.hpp"

enum class COMPLETION
{
	ACCEPT, RECV, SEND, PLAYER_MOVE
};

class OVERLAPPEDEX
{
public:
	OVERLAPPEDEX();

	void Set(char* packet);
	void Reset();

public:
	OVERLAPPED over;
	WSABUF wsa;
	COMPLETION type;
	int target_id;
	char data[VAR::DATA];
};

#endif // !_OVERLAPPEDEX_HPP_
