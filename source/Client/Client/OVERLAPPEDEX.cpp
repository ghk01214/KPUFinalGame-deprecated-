#include "pch.h"
#include "OVERLAPPEDEX.h"

OVERLAPPEDEX::OVERLAPPEDEX() :
	type(COMPLETION::RECV),
	wsa({ VAR::DATA, data })
{
	ZeroMemory(&over, sizeof(over));
}

void OVERLAPPEDEX::Set(char* packet)
{
	wsa.len = static_cast<ULONG>(packet[0]);
	wsa.buf = data;
	type = COMPLETION::SEND;

	ZeroMemory(&over, sizeof(over));
	std::memcpy(data, packet, packet[0]);
}

void OVERLAPPEDEX::Reset()
{
	wsa.len = VAR::DATA;
	wsa.buf = data;
	type = COMPLETION::RECV;

	ZeroMemory(&over, sizeof(over));
	data[0] = '\0';
}
