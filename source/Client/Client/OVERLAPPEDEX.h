#pragma once

#include "../../Server/Server/protocol.hpp"

class OVERLAPPEDEX
{
public:
	OVERLAPPEDEX();

	void SetPacket(char* packet);

public:
	OVERLAPPED over;
	WSABUF wsa_buf;
	char data[VAR_SIZE::DATA];
	char type;
};
