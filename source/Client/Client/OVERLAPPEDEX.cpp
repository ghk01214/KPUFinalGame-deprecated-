#include "pch.h"
#include "OVERLAPPEDEX.h"

OVERLAPPEDEX::OVERLAPPEDEX() : type(static_cast<char>(COMPLETION_TYPE::RECV)), wsa_buf({ VAR_SIZE::DATA, data })
{
	ZeroMemory(&over, sizeof(over));
}

void OVERLAPPEDEX::SetPacket(char* packet)
{
	type = static_cast<char>(COMPLETION_TYPE::SEND);
	wsa_buf.len = static_cast<ULONG>(packet[0]);
	wsa_buf.buf = data;

	ZeroMemory(&over, sizeof(over));
	std::memcpy(data + 2, packet, packet[0]);
}
