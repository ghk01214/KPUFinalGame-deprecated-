#ifndef _OVERLAPPEDEX_HPP_
#define _OVERLAPPEDEX_HPP_

#include "protocol.hpp"

enum class COMPLETION_TYPE
{
	ACCEPT, RECV, SEND
};

class OVERLAPPEDEX
{
public:
	OVERLAPPEDEX();

	void Set(char* packet);

public:
	OVERLAPPED over;
	WSABUF wsa_buf;
	char data[VAR_SIZE::DATA];
	char type;
};

#endif // !_OVERLAPPEDEX_HPP_
