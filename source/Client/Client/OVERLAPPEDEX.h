#pragma once

#include "../Server/protocol.hpp"

enum class COMPLETION_TYPE
{
	RECV, SEND
};

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