#pragma once

#include "../../Server/Server/protocol.hpp"

enum class COMPLETION
{
	RECV, SEND
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