#include "pch.h"
#include "GameFramework.h"
#include "Network.h"

CNetwork::CNetwork(CGameFramework* game_inst) :
	server(INVALID_SOCKET),
	iocp(INVALID_HANDLE_VALUE),
	key(9999),
	remain_size(0),
	game_instance(game_inst),
	sc_login(new SC::P::LOGIN),
	sc_move_object(new SC::P::MOVE_OBJ),
	sc_add_object(new SC::P::ADD_OBJ),
	sc_remove_object(new SC::P::DELETE_OBJ),
	cs_login(new CS::P::LOGIN),
	cs_move_object(new CS::P::MOVE_OBJ),
	cs_rotate_object(new CS::P::ROTATE_OBJ),
	cs_player_attack(new CS::P::PLAYER_ATTACK),
	over_ex(nullptr),
	packet(nullptr)
{
}

CNetwork::~CNetwork()
{
	closesocket(server);
	WSACleanup();

	worker_thread.join();

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
	if (sc_login)
	{
		delete sc_login;
		sc_login = nullptr;
	}
	if (sc_move_object)
	{
		delete sc_move_object;
		sc_move_object = nullptr;
	}
	if (sc_add_object)
	{
		delete sc_add_object;
		sc_add_object = nullptr;
	}
	if (sc_remove_object)
	{
		delete sc_remove_object;
		sc_remove_object = nullptr;
	}
	if (cs_login)
	{
		delete cs_login;
		cs_login = nullptr;
	}
	if (cs_move_object)
	{
		delete cs_move_object;
		cs_move_object = nullptr;
	}
	if (cs_rotate_object)
	{
		delete cs_rotate_object;
		cs_rotate_object = nullptr;
	}
	if (cs_player_attack)
	{
		delete cs_player_attack;
		cs_player_attack = nullptr;
	}
	if (game_instance)
	{
		game_instance = nullptr;
	}
}

void CNetwork::ConnectToServer()
{
	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != NOERROR)
	{
		ErrorQuit(L"WSAStartup fuction error");
	}

	if (iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, NULL);
		iocp == INVALID_HANDLE_VALUE)
	{
		ErrorQuit(L"IOCP Handle Creation Failed");
	}

	server = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);

	sockaddr_in server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	InetPton(AF_INET, SERVER_ADDR.c_str(), &server_addr.sin_addr);

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(server), iocp, key, 0);

	if (WSAConnect(server, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr), 0, 0, 0, 0) == SOCKET_ERROR)
	{
		ErrorQuit(L"Connection Failed");
	}

	worker_thread = std::thread{ &CNetwork::ProcessThread, this };
}

void CNetwork::ProcessThread()
{
	DWORD bytes;
	ULONG_PTR id;
	BOOL ret;

	while (true)
	{
		if (GetQueuedCompletionStatus(iocp, &bytes, &id, reinterpret_cast<LPOVERLAPPED*>(&over_ex), INFINITE) == 0)
		{
			ErrorQuit(L"GetQueuedCompletionStatus error");
		}

		switch (over_ex->type)
		{
		case COMPLETION::RECV:
		{
			RecvData(bytes);
		}
		break;
		case COMPLETION::SEND:
		{
			SendData(bytes);
		}
		break;
		case COMPLETION::QUIT:
		{
			return;
		}
		break;
		}
	}
}

void CNetwork::EndThread()
{
	over_ex->type = COMPLETION::QUIT;

	PostQueuedCompletionStatus(iocp, 1, key, &over_ex->over);
}

void CNetwork::RecvData()
{
	DWORD flag{ 0 };
	ZeroMemory(&recv_ex.over, sizeof(recv_ex.over));

	recv_ex.wsa.len = VAR::DATA - remain_size;
	recv_ex.wsa.buf = recv_ex.data + remain_size;

	WSARecv(server, &recv_ex.wsa, 1, 0, &flag, &recv_ex.over, nullptr);
}

void CNetwork::SendData(void* pack)
{
	send_ex.Set(reinterpret_cast<char*>(pack));

	WSASend(server, &send_ex.wsa, 1, 0, 0, &send_ex.over, nullptr);
}

void CNetwork::RecvData(DWORD bytes)
{
	packet = over_ex->data;

	int remain{ static_cast<int>(bytes) + remain_size };
	int packet_size{ packet[0] };

	while (remain > 0)
	{
		packet_size = packet[0];

		if (packet_size <= remain)
		{
			ProcessPacket();

			packet += packet_size;
			remain -= packet_size;
		}
		else
			break;
	}

	//for (; remain > 0 && packet_size <= remain; remain -= packet_size)
	//{
	//	packet_size = packet[0];
	//
	//	ProcessPacket();
	//
	//	packet += packet_size;
	//}

	remain_size = remain;

	if (remain > 0)
	{
		std::memcpy(over_ex->data, packet, remain);
	}

	RecvData();
}

void CNetwork::SendData(DWORD bytes)
{
	//if (!bytes)
	//{
	//	return;
	//}

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
	case SC::MOVE_OBJ:
	{
		ProcessMovePacket();
	}
	break;
	case SC::ADD_OBJ:
	{
		ProcessAddObjectPacket();
	}
	break;
	case SC::DELETE_OBJ:
	{
		ProcessRemoveObjectPacket();
	}
	break;
	}
}

void CNetwork::ProcessLoginPacket()
{
	sc_login = reinterpret_cast<SC::P::LOGIN*>(packet);

	auto player{ game_instance->GetPlayers()->extract(game_instance->GetPlayers()->begin()) };
	player.key() = sc_login->id;
	game_instance->GetPlayers()->insert(std::move(player));

	float x{ static_cast<float>(sc_login->x) };
	float y{ static_cast<float>(sc_login->y) };
	float z{ static_cast<float>(sc_login->z) };

	XMFLOAT3 temp{ x, y, z };

	game_instance->GetPlayer(sc_login->id)->SetPosition(temp);

	std::cout << "my uid is [" << sc_login->id << "]" << std::endl;
}

void CNetwork::ProcessMovePacket()
{
	sc_move_object = reinterpret_cast<SC::P::MOVE_OBJ*>(packet);

	auto player{ game_instance->GetPlayer(sc_move_object->id) };

	player->Move(sc_move_object->x, sc_move_object->y, sc_move_object->z);
	player->Update(game_instance->GetTimer()->GetTimeElapsed());
}

void CNetwork::ProcessAddObjectPacket()
{
	sc_add_object = reinterpret_cast<SC::P::ADD_OBJ*>(packet);

	game_instance->AddPlayer(sc_add_object);

	std::cout << "player[" << sc_add_object->id << "] is in sight" << std::endl;
}

void CNetwork::ProcessRemoveObjectPacket()
{
	sc_remove_object = reinterpret_cast<SC::P::DELETE_OBJ*>(packet);

	//game_instance->GetPlayers()->erase(sc_remove_object->id);
	game_instance->RemovePlayer(sc_remove_object->id);

	std::cout << "player[" << sc_remove_object->id << "] is out of sight" << std::endl;
}

void CNetwork::SendLoginPacket()
{
	cs_login->size = sizeof(CS::P::LOGIN);
	cs_login->type = CS::LOGIN;
	strcpy_s(cs_login->name, "player");

	cs_login->look_x = 0.0f;
	cs_login->look_y = 0.0f;
	cs_login->look_z = 1.0f;

	cs_login->right_x = 1.0f;
	cs_login->right_y = 0.0f;
	cs_login->right_z = 0.0f;

	cs_login->up_x = 0.0f;
	cs_login->up_y = 1.0f;
	cs_login->up_z = 0.0f;

	cs_login->pitch = 0.0f;
	cs_login->yaw = 0.0f;

	SendData(cs_login);
}

void CNetwork::SendMoveObjectPacket(DWORD direction)
{
	cs_move_object->size = sizeof(CS::P::MOVE_OBJ);
	cs_move_object->type = CS::MOVE_OBJ;
	cs_move_object->direction = static_cast<char>(direction);

	SendData(cs_move_object);
}

void CNetwork::SendRotateObjectPacket(float cx, float cy)
{
	cs_rotate_object->size = sizeof(CS::P::ROTATE_OBJ);
	cs_rotate_object->type = CS::ROTATE_OBJ;
	cs_rotate_object->cx = -cx;
	cs_rotate_object->cy = -cy;

	SendData(cs_rotate_object);
}

void CNetwork::SendPlayerAttackPacket(int mode)
{
	cs_player_attack->size = sizeof(CS::P::PLAYER_ATTACK);
	cs_player_attack->type = CS::PLAYER_ATTACK;
	cs_player_attack->mode = static_cast<char>(mode);

	SendData(cs_player_attack);
}
