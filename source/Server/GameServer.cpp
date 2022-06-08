#include "pch.h"
#include "Lua.h"
#include "NPC.h"
#include "Player.h"
#include "GameServer.h"

std::uniform_real_distribution<POS> random_x{ VAR::WORLD_X_MIN, VAR::WORLD_X_MAX };
std::uniform_real_distribution<POS> random_z{ VAR::WORLD_Z_MIN, VAR::WORLD_Z_MAX };

GameServer::GameServer() :
	iocp{ INVALID_HANDLE_VALUE },
	server{ INVALID_SOCKET },
	server_key{ 99999 },
	packet{ nullptr },
	over_ex{ nullptr },
	cs_login{ nullptr },
	cs_move{ nullptr },
	cs_rotate{ nullptr },
	cs_player_attack{ nullptr },
	zone{ new Zone{} }
{
	for (int i = 0; i < MAX_USER; ++i)
	{
		clients[i] = new Session{ new Player{} };
	}

	//for (int id = NPC_START; id < NPC_NUM; ++id)
	//{
	//	clients[id] = new Session{ new NPC{} };
	//}
}

GameServer::~GameServer()
{
	closesocket(server);
	WSACleanup();

	if (!clients.empty())
	{
		for (auto& client : clients)
		{
			delete client;
			client = nullptr;
		}
	}

	if (over_ex)
	{
		delete over_ex;
		over_ex = nullptr;
	}
	if (packet)
	{
		delete packet;
		packet = nullptr;
	}
	if (zone)
	{
		delete zone;
		zone = nullptr;
	}
	if (cs_login)
	{
		delete cs_login;
		cs_login = nullptr;
	}
	if (cs_move)
	{
		delete cs_move;
		cs_move = nullptr;
	}
	if (cs_rotate)
	{
		delete cs_rotate;
		cs_rotate = nullptr;
	}
	if (cs_player_attack)
	{
		delete cs_player_attack;
		cs_player_attack = nullptr;
	}
}

void GameServer::Run()
{
	Initialize();
	Accept();
	CreateThread();
}

void GameServer::Initialize()
{
	if (WSADATA wsa; WSAStartup(MAKEWORD(2, 2), &wsa) != NOERROR)
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
	ZeroMemory(&server_addr, sizeof(sockaddr_in));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(server), iocp, server_key, 0);

	if (bind(server, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR)
	{
		ErrorQuit(L"bind function error");
	}

	if (listen(server, SOMAXCONN) == SOCKET_ERROR)
	{
		ErrorQuit(L"listen function error");
	}
}

void GameServer::InitializeNPC()
{
	std::cout << "NPC Initialization Start" << std::endl;

	Lua::MakeNewInstance(zone);

	for (int id = NPC_START; id < NPC_NUM; ++id)
	{
		clients[id]->SetState(STATE::INGAME);
		clients[id]->SetObjectPos(random_x(dre), 270.0f, random_z(dre));
		clients[id]->SetObjectName("NPC-" + std::to_string(id));
		clients[id]->SetID(id);

		zone->AddObject(clients[id]);

		//dynamic_cast<NPC*>(clients[id]->GetMyObject())->InitializeScript(id);
	}

	std::cout << "Done" << std::endl;
}

void GameServer::Accept()
{
	SOCKET client_socket{ WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED) };

	OVERLAPPEDEX accept_ex;
	accept_ex.type = COMPLETION::ACCEPT;
	accept_ex.wsa.buf = reinterpret_cast<char*>(client_socket);

	AcceptEx(server, client_socket, accept_ex.data, 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, 0, &accept_ex.over);
}

void GameServer::CreateThread()
{
	for (int i = 0; i < MAX_USER; ++i)
	{
		worker_threads.emplace_back(&GameServer::WorkerThread, this);
	}

	//ai_thread = std::thread{ &GameServer::AIThread, this };

	//ai_thread.join();

	for (auto& thread : worker_threads)
	{
		thread.join();
	}
}

void GameServer::WorkerThread()
{
	DWORD bytes;
	ULONG_PTR id;

	while (true)
	{
		if (GetQueuedCompletionStatus(iocp, &bytes, &id, reinterpret_cast<LPOVERLAPPED*>(&over_ex), INFINITE) == FALSE)
		{
			if (over_ex->type == COMPLETION::ACCEPT)
			{
				DisplayError(L"Accept error");
			}
			else
			{
				DisplayError(L"GQCS Error on client[" + std::to_wstring(id) + L"]");
				Disconnect(id);

				if (over_ex->type == COMPLETION::SEND)
				{
					over_ex->Reset();
				}

				continue;
			}
		}

		switch (over_ex->type)
		{
		case COMPLETION::ACCEPT:
		{
			AcceptClient();
		}
		break;
		case COMPLETION::RECV:
		{
			Recv(id, bytes);
		}
		break;
		case COMPLETION::SEND:
		{
			Send(id, bytes);
		}
		break;
		}
	}
}

void GameServer::AcceptClient()
{
	SOCKET client_socket{ reinterpret_cast<SOCKET>(over_ex->wsa.buf) };

	if (int id = NewPlayerID(); id != -1)
	{
		clients[id]->SetID(id);
		clients[id]->remain = 0;
		clients[id]->SetSocket(client_socket);

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_socket), iocp, id, 0);

		clients[id]->Recv();

		client_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	}
	else
	{
		std::cout << "Max user exceeded" << std::endl;
	}

	ZeroMemory(&over_ex->over, sizeof(over_ex->over));
	over_ex->wsa.buf = reinterpret_cast<char*>(client_socket);

	AcceptEx(server, client_socket, over_ex->data, 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, 0, &over_ex->over);
}

void GameServer::Recv(ULONG_PTR id, DWORD bytes)
{
	if (bytes == 0)
	{
		Disconnect(id);
		return;
	}

	int remain_size{ static_cast<int>(bytes) + clients[id]->remain };
	int packet_size{};

	packet = over_ex->data;

	while (remain_size > 0)
	{
		packet_size = packet[0];

		if (packet_size <= remain_size)
		{
			ProcessPacket(id);

			packet += packet_size;
			remain_size -= packet_size;
		}
		else
			break;
	}

	clients[id]->remain = remain_size;

	if (remain_size > 0)
	{
		memcpy_s(over_ex->data, VAR::DATA, packet, remain_size);
	}

	clients[id]->Recv();
}

void GameServer::Send(ULONG_PTR id, DWORD bytes)
{
	if (bytes == 0)
	{
		Disconnect(id);
	}

	over_ex->Reset();
}

int GameServer::NewPlayerID()
{
	for (int id = 0; id < MAX_USER; ++id)
	{
		if (clients[id]->GetState() == STATE::FREE)
		{
			clients[id]->SetState(STATE::ACCEPTED);

			return id;
		}
	}

	return -1;
}

int GameServer::NewRandomID()
{
	return 0;
}

void GameServer::Disconnect(ULONG_PTR id)
{
	if (clients[id]->GetState() == STATE::FREE)
	{
		return;
	}

	clients[id]->Reset();

	for (auto& client : clients)
	{
		if (client->IsMyID(id))
		{
			continue;
		}

		if (client->GetState() != STATE::INGAME)
		{
			continue;
		}

		// view list에 삭제하려는 object가 있으면
		if (client->IsInViewList(id))
		{
			// view list에서 지우고 클라이언트에 제거 패킷 전송
			client->RemoveFromViewList(id);
			client->SendRemove(id);
		}
	}
}

void GameServer::ProcessPacket(ULONG_PTR id)
{
	switch (packet[1])		// packet type
	{
	case CS::LOGIN:
	{
		Login(id);
	}
	break;
	case CS::MOVE_OBJ:
	{
		Move(id);
	}
	break;
	case CS::ROTATE_OBJ:
	{
		Rotate(id);
	}
	break;
	case CS::PLAYER_ATTACK:
	{
		PlayerAttack(id);
	}
	break;
	}
}

void GameServer::Login(ULONG_PTR id)
{
	cs_login = reinterpret_cast<CS::P::LOGIN*>(packet);

	XMFLOAT3 look{ cs_login->look_x, cs_login->look_y, cs_login->look_z };
	XMFLOAT3 right{ cs_login->right_x, cs_login->right_y, cs_login->right_z };
	XMFLOAT3 up{ cs_login->up_x, cs_login->up_y, cs_login->up_z };

	if (clients[id]->GetState() == STATE::FREE)
	{
		return;
	}

	if (clients[id]->GetState() == STATE::INGAME)
	{
		Disconnect(id);
		return;
	}

	clients[id]->SetObjectName(cs_login->name);
	dynamic_cast<Player*>(clients[id]->GetMyObject())->SetLook(look);
	dynamic_cast<Player*>(clients[id]->GetMyObject())->SetRight(right);
	dynamic_cast<Player*>(clients[id]->GetMyObject())->SetUp(up);
	dynamic_cast<Player*>(clients[id]->GetMyObject())->SetPitch(cs_login->pitch);
	dynamic_cast<Player*>(clients[id]->GetMyObject())->SetYaw(cs_login->yaw);
	clients[id]->SetObjectPos(random_x(dre), 270.0f, random_z(dre));
	clients[id]->SendLogin();
	clients[id]->SetState(STATE::INGAME);

	zone->AddObject(clients[id]);

	for (int i = 0; i < MAX_USER; ++i)
	{
		if (clients[i]->IsMyID(id))
		{
			continue;
		}

		if (clients[i]->GetState() != STATE::INGAME)
		{
			continue;
		}

		auto dis1{ clients[id]->GetX() - clients[i]->GetX() };
		auto dis2{ clients[id]->GetZ() - clients[i]->GetZ() };

		if (::IsInSight(dis1, dis2))
		{
			clients[i]->AddToViewList(id);
			clients[i]->SendAdd(clients[id]);
		}
	}

	for (auto& player : clients)
	{
		if (player->IsMyID(id))
		{
			continue;
		}

		if (player->GetState() != STATE::INGAME)
		{
			continue;
		}

		auto dis1{ clients[id]->GetX() - player->GetX() };
		auto dis2{ clients[id]->GetY() - player->GetY() };

		if (::IsInSight(dis1, dis2))
		{
			clients[id]->AddToViewList(player->GetId());
			clients[id]->SendAdd(player);
		}
	}

	std::cout << "player[" << id << "] login" << std::endl;
}

void GameServer::Move(ULONG_PTR id)
{
	cs_move = reinterpret_cast<CS::P::MOVE_OBJ*>(packet);

	zone->MoveObject(id, cs_move->direction);

	// NPC
	//for (int i = NPC_START; i < NPC_NUM; ++i)
	//{
	//	auto dis1{ clients[i]->GetX() - clients[id]->GetX() };
	//	auto dis2{ clients[i]->GetZ() - clients[id]->GetZ() };
	//
	//	if (::IsInSight(dis1, dis2))
	//	{
	//		over_ex->type = COMPLETION::PLAYER_MOVE;
	//		over_ex->target = id;
	//		PostQueuedCompletionStatus(iocp, 1, i, &over_ex->over);
	//	}
	//}
}

void GameServer::Rotate(ULONG_PTR id)
{
	cs_rotate = reinterpret_cast<CS::P::ROTATE_OBJ*>(packet);

	zone->RotateObject(id, cs_rotate->cx, cs_rotate->cy);
}

void GameServer::PlayerAttack(ULONG_PTR id)
{
}

void GameServer::AIThread()
{
	while (true)
	{
		for (int id = NPC_START; id < NPC_NUM; ++id)
		{
			dynamic_cast<NPC*>(clients[id]->GetMyObject())->Move();
		}
	}
}
