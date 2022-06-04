#pragma once

#include "../Server/protocol.h"

enum class COMPLETION
{
	RECV, SEND, QUIT
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
	char data[VAR::DATA];
};