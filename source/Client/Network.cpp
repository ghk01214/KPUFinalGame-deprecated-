#include "pch.h"
#include "GameFramework.h"
#include "Network.h"

CNetwork::CNetwork(CGameFramework* game_inst) :
	server(INVALID_SOCKET),
	iocp(INVALID_HANDLE_VALUE),
	remain_size(0),
	game_instance(game_inst),
	sc_login_packet(new SC::PACKET::LOGIN),
	sc_move_object_packet(new SC::PACKET::MOVE_OBJECT),
	sc_add_object_packet(new SC::PACKET::ADD_OBJECT),
	sc_remove_object_packet(new SC::PACKET::REMOVE_OBJECT),
	cs_login_packet(new CS::PACKET::LOGIN),
	cs_move_object_packet(new CS::PACKET::MOVE_OBJECT),
	cs_rotate_object_packet(new CS::PACKET::ROTATE_OBJECT),
	cs_player_attack_packet(new CS::PACKET::PLAYER_ATTACK),
	over_ex(nullptr),
	packet(nullptr)
{
}

CNetwork::~CNetwork()
{
	if (packet)
	{
		delete packet;
		packet = nullptr;
	}
	if (over_ex)
	{
		delete over_ex;
		over_ex = nullptr;
	}
	if (sc_login_packet)
	{
		delete sc_login_packet;
		sc_login_packet = nullptr;
	}
	if (sc_move_object_packet)
	{
		delete sc_move_object_packet;
		sc_move_object_packet = nullptr;
	}
	if (sc_add_object_packet)
	{
		delete sc_add_object_packet;
		sc_add_object_packet = nullptr;
	}
	if (sc_remove_object_packet)
	{
		delete sc_remove_object_packet;
		sc_remove_object_packet = nullptr;
	}
	if (cs_login_packet)
	{
		delete cs_login_packet;
		cs_login_packet = nullptr;
	}
	if (cs_move_object_packet)
	{
		delete cs_move_object_packet;
		cs_move_object_packet = nullptr;
	}
	if (cs_rotate_object_packet)
	{
		delete cs_rotate_object_packet;
		cs_rotate_object_packet = nullptr;
	}
	if (cs_player_attack_packet)
	{
		delete cs_player_attack_packet;
		cs_player_attack_packet = nullptr;
	}
	if (game_instance)
	{
		game_instance = nullptr;
	}

	closesocket(server);
	WSACleanup();
}

void CNetwork::ConnectToServer()
{
	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != NOERROR)
	{
		ErrorQuit(L"WSAStartup fuction error");
	}

	if (iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, NULL);
		iocp == nullptr)
	{
		ErrorQuit(L"IOCP Handle Creation Failed");
	}

	server = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);

	sockaddr_in server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	InetPton(AF_INET, SERVER_ADDR.c_str(), &server_addr.sin_addr);

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(server), iocp, server, 0);

	if (WSAConnect(server, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr), 0, 0, 0, 0) == SOCKET_ERROR)
	{
		ErrorQuit(L"Connection Failed");
	}

	worker_thread = std::thread{ &CNetwork::ProcessThread, this };
}

void CNetwork::ProcessThread()
{
	DWORD bytes;
	ULONG_PTR key;
	BOOL ret;

	while (true)
	{
		ret = GetQueuedCompletionStatus(iocp, &bytes, &key, reinterpret_cast<LPOVERLAPPED*>(&over_ex), INFINITE);

		if (ret == 0)
		{
			ErrorQuit(L"GetQueuedCompletionStatus error");
		}

		switch (over_ex->type)
		{
		case static_cast<int>(COMPLETION_TYPE::RECV):
		{
			RecvData(bytes);
		}
		break;
		case static_cast<int>(COMPLETION_TYPE::SEND):
		{
			SendData(bytes);
		}
		break;
		}
	}
}

void CNetwork::RecvData()
{
	DWORD flag{ 0 };
	ZeroMemory(&recv_ex.over, sizeof(recv_ex.over));

	recv_ex.wsa_buf.buf = recv_ex.data + remain_size;
	recv_ex.wsa_buf.len = VAR_SIZE::DATA - remain_size;

	WSARecv(server, &recv_ex.wsa_buf, 1, 0, &flag, &recv_ex.over, nullptr);
}

void CNetwork::SendData(void* pack)
{
	send_ex.SetPacket(reinterpret_cast<char*>(pack));

	WSASend(server, &send_ex.wsa_buf, 1, 0, 0, &send_ex.over, nullptr);
}

void CNetwork::RecvData(DWORD bytes)
{
	if (!bytes)
	{
		return;
	}

	int remain{ static_cast<int>(bytes) + remain_size };
	int packet_size{ over_ex->data[0] };

	packet = over_ex->data;

	for (; remain > 0 || packet_size <= remain; remain -= packet_size)
	{
		packet_size = packet[0];

		ProcessPacket();

		packet += packet_size;
	}

	remain_size = remain;

	if (remain > 0)
	{
		std::memcpy(over_ex->data, packet, remain);
	}

	RecvData();
}

void CNetwork::SendData(DWORD bytes)
{
	if (!bytes)
	{
		return;
	}

	ZeroMemory(&over_ex, sizeof(over_ex));
	over_ex = nullptr;
}

void CNetwork::ProcessPacket()
{
	int packet_type{ packet[1] };

	switch (packet_type)
	{
	case SC::LOGIN:
	{
		ProcessLoginPacket();
	}
	break;
	case SC::MOVE_OBJECT:
	{
		ProcessMovePacket();
	}
	break;
	case SC::ADD_OBJECT:
	{
		ProcessAddObjectPacket();
	}
	break;
	case SC::REMOVE_OBJECT:
	{
		ProcessRemoveObjectPacket();
	}
	break;
	}
}

void CNetwork::ProcessLoginPacket()
{
	sc_login_packet = reinterpret_cast<SC::PACKET::LOGIN*>(packet);

	auto player{ game_instance->GetPlayers()->extract(game_instance->GetPlayers()->begin()) };
	player.key() = sc_login_packet->id;
	game_instance->GetPlayers()->insert(std::move(player));

	float x{ static_cast<float>(sc_login_packet->x) };
	float y{ static_cast<float>(sc_login_packet->y) };
	float z{ static_cast<float>(sc_login_packet->z) };

	XMFLOAT3 temp{ x, y, z };

	game_instance->GetPlayer(sc_login_packet->id)->SetPosition(temp);
}

void CNetwork::ProcessMovePacket()
{
	sc_move_object_packet = reinterpret_cast<SC::PACKET::MOVE_OBJECT*>(packet);

	auto player{ game_instance->GetPlayer(sc_move_object_packet->id) };

	player->Move(sc_move_object_packet->x, sc_move_object_packet->y, sc_move_object_packet->z);
	player->Update(game_instance->GetTimer()->GetTimeElapsed());
}

void CNetwork::ProcessAddObjectPacket()
{
	sc_add_object_packet = reinterpret_cast<SC::PACKET::ADD_OBJECT*>(packet);

	game_instance->AddPlayer(sc_add_object_packet);
}

void CNetwork::ProcessRemoveObjectPacket()
{
	sc_remove_object_packet = reinterpret_cast<SC::PACKET::REMOVE_OBJECT*>(packet);

	game_instance->GetPlayers()->erase(sc_remove_object_packet->id);
}

void CNetwork::SendLoginPacket()
{
	cs_login_packet->size = sizeof(CS::PACKET::LOGIN);
	cs_login_packet->type = CS::LOGIN;
	strcpy_s(cs_login_packet->name, "player");
	cs_login_packet->look_x = 0.0f;
	cs_login_packet->look_y = 0.0f;
	cs_login_packet->look_z = 1.0f;

	cs_login_packet->right_x = 1.0f;
	cs_login_packet->right_y = 0.0f;
	cs_login_packet->right_z = 0.0f;

	cs_login_packet->up_x = 0.0f;
	cs_login_packet->up_y = 1.0f;
	cs_login_packet->up_z = 0.0f;

	SendData(cs_login_packet);
}

void CNetwork::SendMoveObjectPacket(DWORD direction, float look_x, float look_z, float right_x, float right_z)
{
	cs_move_object_packet->size = sizeof(CS::PACKET::MOVE_OBJECT);
	cs_move_object_packet->type = CS::MOVE_OBJECT;
	cs_move_object_packet->direction = static_cast<char>(direction);

	SendData(cs_move_object_packet);
}

void CNetwork::SendRotateObjectPacket(float cy)
{
	cs_rotate_object_packet->size = sizeof(CS::PACKET::ROTATE_OBJECT);
	cs_rotate_object_packet->type = CS::ROTATE_OBJECT;
	cs_rotate_object_packet->cy = cy;

	SendData(cs_rotate_object_packet);
}

void CNetwork::SendPlayerAttackPacket(int mode)
{
	cs_player_attack_packet->size = sizeof(CS::PACKET::PLAYER_ATTACK);
	cs_player_attack_packet->type = CS::PLAYER_ATTACK;
	cs_player_attack_packet->mode = static_cast<char>(mode);

	SendData(cs_player_attack_packet);
}
