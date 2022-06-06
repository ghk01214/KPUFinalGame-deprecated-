#include "pch.h"
#include "OVERLAPPEDEX.h"

OVERLAPPEDEX::OVERLAPPEDEX() :
	wsa{ VAR::DATA, data },
	type{ COMPLETION::RECV },
	target{ -1 }
{
	ZeroMemory(&over, sizeof(over));
}

void OVERLAPPEDEX::Set(char* packet)
{
	type = COMPLETION::SEND;
	wsa.len = static_cast<ULONG>(packet[0]);
	wsa.buf = data;

	ZeroMemory(&over, sizeof(over));
	memcpy_s(data, VAR::DATA, packet, packet[0]);
}

void OVERLAPPEDEX::Reset()
{
	wsa.len = VAR::DATA;
	wsa.buf = data;
	type = COMPLETION::RECV;
	target = -1;
	data[0] = '\0';

	ZeroMemory(&over, sizeof(over));
}
