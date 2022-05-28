#include "pch.hpp"
#include "OVERLAPPEDEX.hpp"

OVERLAPPEDEX::OVERLAPPEDEX() :
	wsa{ VAR::DATA, data },
	type{ COMPLETION::RECV },
	target_id{ -1 }
{
	ZeroMemory(&over, sizeof(over));
}

void OVERLAPPEDEX::Set(char* packet)
{
	type = COMPLETION::SEND;
	wsa.len = static_cast<ULONG>(packet[0]);
	wsa.buf = data;

	ZeroMemory(&over, sizeof(over));
	std::memcpy(data, packet, packet[0]);
}

void OVERLAPPEDEX::Reset()
{
	wsa.len = VAR::DATA;
	wsa.buf = data;
	type = COMPLETION::RECV;
	target_id = -1;

	ZeroMemory(&over, sizeof(over));
	data[0] = '\0';
}
