#include "pch.h"
#include "Player.h"
#include "GameFramework.h"
#include "Network.h"

CNetwork::CNetwork(CGameFramework* game_inst) :
	server(INVALID_SOCKET),
	iocp(INVALID_HANDLE_VALUE),
	key(9999),
	remain_size(0),
	game_instance(game_inst),
	sc_login(nullptr),
	sc_add_object(nullptr),
	sc_remove_object(nullptr),
	sc_move_object(nullptr),
	sc_rotate_object(nullptr),
	packet(nullptr)
{
}

CNetwork::~CNetwork()
{
	closesocket(server);
	WSACleanup();

	worker_thread.join();

	if (packet[0] != '\0')
	{
		delete packet;
		packet = nullptr;
	}
	else
	{
		packet = nullptr;
	}
	if (sc_login)
	{
		sc_login = nullptr;
	}
	if (sc_move_object)
	{
		sc_move_object = nullptr;
	}
	if (sc_add_object)
	{
		sc_add_object = nullptr;
	}
	if (sc_remove_object)
	{
		sc_remove_object = nullptr;
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
	OVERLAPPEDEX* over_ex{ nullptr };

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
			Recv(bytes, over_ex);
		}
		break;
		case COMPLETION::SEND:
		{
			Send(bytes, over_ex);
		}
		break;
		case COMPLETION::QUIT:
		{
			return;
		}
		}
	}
}

void CNetwork::EndThread()
{
	OVERLAPPEDEX over_ex;
	over_ex.type = COMPLETION::QUIT;

	PostQueuedCompletionStatus(iocp, 1, key, &over_ex.over);
}

void CNetwork::Recv()
{
	DWORD flag{ 0 };
	ZeroMemory(&recv_ex.over, sizeof(recv_ex.over));

	recv_ex.wsa.len = VAR::DATA - remain_size;
	recv_ex.wsa.buf = recv_ex.data + remain_size;

	WSARecv(server, &recv_ex.wsa, 1, 0, &flag, &recv_ex.over, nullptr);
}

void CNetwork::Send(void* pack)
{
	send_ex.Set(reinterpret_cast<char*>(pack));

	WSASend(server, &send_ex.wsa, 1, 0, 0, &send_ex.over, nullptr);
}

void CNetwork::Recv(DWORD bytes, OVERLAPPEDEX* over_ex)
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
		memcpy_s(over_ex->data, VAR::DATA, packet, remain);
	}

	Recv();
}

void CNetwork::Send(DWORD bytes, OVERLAPPEDEX* over_ex)
{
	ZeroMemory(&over_ex->over, sizeof(over_ex->over));
	over_ex = nullptr;

	//over_ex->Reset();			// 왜 이건 안 되고 위는 되지???
}

void CNetwork::ProcessPacket()
{
	int packet_type{ packet[1] };

	switch (packet_type)
	{
	case SC::LOGIN:
	{
		Login();
	}
	break;
	case SC::ADD_OBJ:
	{
		AddObject();
	}
	break;
	case SC::REMOVE_OBJ:
	{
		RemoveObject();
	}
	break;
	case SC::MOVE_OBJ:
	{
		MoveObject();
	}
	break;
	case SC::ROTATE_OBJ:
	{
		RotateObject();
	}
	break;
	}
}

void CNetwork::Login()
{
	sc_login = reinterpret_cast<SC::P::LOGIN*>(packet);

	auto players{ game_instance->GetPlayers() };

	auto iter{ std::find_if(players->begin(), players->end(), [](const std::pair<int, CPlayer*>& other)
		{
			return other.first < 0;
		}) };

	auto player{ players->extract(iter) };
	player.key() = sc_login->id;
	players->insert(std::move(player));

	float x{ static_cast<float>(sc_login->x) };
	float y{ static_cast<float>(sc_login->y) };
	float z{ static_cast<float>(sc_login->z) };

	XMFLOAT3 temp{ x, y, z };

	(*players)[sc_login->id]->SetPosition(temp);

	std::cout << "my uid is [" << sc_login->id << "]" << std::endl;
}

void CNetwork::AddObject()
{
	sc_add_object = reinterpret_cast<SC::P::ADD_OBJ*>(packet);

	game_instance->AddPlayer(sc_add_object);

	std::cout << "player[" << sc_add_object->id << "] is in sight" << std::endl;
}

void CNetwork::RemoveObject()
{
	sc_remove_object = reinterpret_cast<SC::P::REMOVE_OBJ*>(packet);

	game_instance->RemovePlayer(sc_remove_object->id);

	std::cout << "player[" << sc_remove_object->id << "] is out of sight" << std::endl;
}

void CNetwork::MoveObject()
{
	sc_move_object = reinterpret_cast<SC::P::MOVE_OBJ*>(packet);

	auto player{ game_instance->GetPlayer(sc_move_object->id) };

	player->Move(sc_move_object->x, sc_move_object->y, sc_move_object->z);
	player->Update(game_instance->GetTimer()->GetTimeElapsed());
}

void CNetwork::RotateObject()
{
	sc_rotate_object = reinterpret_cast<SC::P::ROTATE_OBJ*>(packet);

	auto player{ game_instance->GetPlayer(sc_rotate_object->id) };

	player->SetLookVector(XMFLOAT3{ sc_rotate_object->look_x, sc_rotate_object->look_y, sc_rotate_object->look_z });
	player->SetRightVector(XMFLOAT3{ sc_rotate_object->right_x, sc_rotate_object->right_y, sc_rotate_object->right_z });
	player->SetUpVector(XMFLOAT3{ sc_rotate_object->up_x, sc_rotate_object->up_y, sc_rotate_object->up_z });
}

void CNetwork::SendLogin()
{
	cs_login.size = sizeof(CS::P::LOGIN);
	cs_login.type = CS::LOGIN;
	strcpy_s(cs_login.name, "player");

	cs_login.look_x = 0.0f;
	cs_login.look_y = 0.0f;
	cs_login.look_z = 1.0f;

	cs_login.right_x = 1.0f;
	cs_login.right_y = 0.0f;
	cs_login.right_z = 0.0f;

	cs_login.up_x = 0.0f;
	cs_login.up_y = 1.0f;
	cs_login.up_z = 0.0f;

	cs_login.pitch = 0.0f;
	cs_login.yaw = 0.0f;

	Send(&cs_login);
}

void CNetwork::SendMoveObject(DWORD direction)
{
	cs_move_object.size = sizeof(CS::P::MOVE_OBJ);
	cs_move_object.type = CS::MOVE_OBJ;
	cs_move_object.direction = static_cast<char>(direction);

	Send(&cs_move_object);
}

void CNetwork::SendRotateObject(float cx, float cy)
{
	cs_rotate_object.size = sizeof(CS::P::ROTATE_OBJ);
	cs_rotate_object.type = CS::ROTATE_OBJ;
	cs_rotate_object.cx = -cx;
	cs_rotate_object.cy = -cy;

	Send(&cs_rotate_object);
}

void CNetwork::SendPlayerAttack(int mode)
{
	cs_player_attack.size = sizeof(CS::P::PLAYER_ATTACK);
	cs_player_attack.type = CS::PLAYER_ATTACK;
	cs_player_attack.mode = static_cast<char>(mode);

	Send(&cs_player_attack);
}
