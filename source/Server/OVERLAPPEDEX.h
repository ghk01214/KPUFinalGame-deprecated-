#pragma once

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
	int target;
	char data[VAR::DATA];
};

