#include "pch.hpp"
#include "Client.h"

CClient::CClient() : id(-1), player(new CPlayer{}), sock(INVALID_SOCKET), state(SESSION_STATE::FREE), remain_size(0)
{
}

CClient::~CClient()
{
	if (player)
	{
		delete player;
		player = nullptr;
	}

	closesocket(sock);
}

void CClient::RecvData()
{
	DWORD flag{ 0 };
	ZeroMemory(&recv_ex.over, sizeof(recv_ex.over));

	recv_ex.wsa_buf.buf = recv_ex.data + remain_size;
	recv_ex.wsa_buf.len = VAR_SIZE::DATA - remain_size;

	WSARecv(sock, &recv_ex.wsa_buf, 1, 0, &flag, &recv_ex.over, nullptr);
}

void CClient::SendData(void* packet)
{
	send_ex.SetPacket(reinterpret_cast<char*>(packet));

	WSASend(sock, &send_ex.wsa_buf, 1, 0, 0, &send_ex.over, nullptr);
}

void CClient::SendLoginPakcet()
{
	sc_login_packet.id = id;
	sc_login_packet.x = player->GetPosX();
	sc_login_packet.y = player->GetPosY();
	sc_login_packet.z = player->GetPosZ();

	SendData(&sc_login_packet);
}

void CClient::SendMovePlayerPacket(int player_id, CPlayer* pl)
{
	sc_move_player_packet.id = player_id;
	sc_move_player_packet.x = pl->GetPosX();
	sc_move_player_packet.y = pl->GetPosY();
	sc_move_player_packet.z = pl->GetPosZ();

	SendData(&sc_move_player_packet);
}

void CClient::SendAddPlayerPacket(int player_id, CClient* client)
{
	sc_add_player_packet.id = player_id;
	strcpy_s(sc_add_player_packet.name, player->GetName());
	sc_add_player_packet.x = player->GetPosX();
	sc_add_player_packet.y = player->GetPosY();

	client->SendData(&sc_add_player_packet);
}

void CClient::SendRemovePlayerPacket(int player_id)
{
	sc_remove_player_packet.id = player_id;

	SendData(&sc_remove_player_packet);
}
