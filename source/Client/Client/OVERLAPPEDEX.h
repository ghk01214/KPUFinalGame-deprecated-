#pragma once

//#include "../../Server/Server/protocol.hpp"
#include "../../../Documents/대학/4학년/졸업작품/KPUFinalGame/source/Server/Server/protocol.hpp"

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