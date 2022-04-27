#include "pch.hpp"
#include "Client.h"

CClient::CClient() : id(-1), player(0, 0, 0), sock(INVALID_SOCKET), state(SESSION_STATE::FREE), remain_size(0)
{
}

void CClient::RecvData()
{
	DWORD flag{ 0 };
	ZeroMemory(&recv_over, sizeof(recv_over));

	recv_over.wsa_buf.buf = recv_over.data + remain_size;
	recv_over.wsa_buf.len = VAR_SIZE::DATA - remain_size;

	WSARecv(sock, &recv_over.wsa_buf, 1, 0, &flag, &recv_over.over, nullptr);
}

void CClient::SendData(void* packet)
{
	send_over.SetPacket(reinterpret_cast<char*>(packet));

	WSASend(sock, &send_over.wsa_buf, 1, 0, 0, &send_over.over, nullptr);
}

void CClient::SendLoginPakcet()
{
	login_packet.id = id;
	login_packet.size = sizeof(SC::PACKET::LOGIN);
	login_packet.type = SC::LOGIN;
	login_packet.x = player.GetPosX();
	login_packet.y = player.GetPosY();

	SendData(&login_packet);
}

void CClient::SendMovePlayerPacket(short plId, char type, CPlayer pl)
{
	pl_move_packet.id = plId;
	pl_move_packet.size = sizeof(SC::PACKET::MOVE_PLAYER);
	pl_move_packet.type = SC::MOVE_PLAYER;
	pl_move_packet.x = pl.GetPosX();
	pl_move_packet.y = pl.GetPosY();

	SendData(&pl_move_packet);
}
