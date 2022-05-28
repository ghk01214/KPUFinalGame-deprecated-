#include "pch.hpp"
#include "protocol.hpp"
#include "OVERLAPPEDEX.hpp"
#include "Session.hpp"
#include "Zone.hpp"
#include "LuaScript.hpp"
#include "NPC.hpp"
#include "Player.hpp"
#include "GameServer.hpp"

std::uniform_int_distribution<int> random_id(0, MAX_USER - 1);
//std::uniform_real_distribution<float> random_x(0.0f, VAR::WORLD_X);
//std::uniform_real_distribution<float> random_z(0.0f, VAR::WORLD_Z);
std::uniform_real_distribution<float> random_x(0.0f, VAR::WORLD_X);
std::uniform_real_distribution<float> random_z(0.0f, VAR::WORLD_Z);

GameServer::GameServer() :
#pragma region
	iocp{ INVALID_HANDLE_VALUE },
	server{ INVALID_SOCKET },
	server_key{ 99999 },
	client_socket{ INVALID_SOCKET },
	over_ex{ nullptr },
	packet{ nullptr },
	cs_login_packet{ new CS::PACKET::LOGIN },
	cs_move_object_packet{ new CS::PACKET::MOVE_OBJECT },
	cs_rotate_object_packet{ new CS::PACKET::ROTATE_OBJECT },
	cs_attack_object_packet{ new CS::PACKET::PLAYER_ATTACK },
	zone{ new Zone{} },
	active_users{ 0 }
{
	for (int i = 0; i < MAX_USER; ++i)
	{
		sessions[i] = new Session{ new Player{} };
	}

	for (int i = NPC_START; i < sessions.size(); ++i)
	{
		sessions[i] = new Session{ new NPC{} };
	}
}
#pragma endregion

GameServer::~GameServer()
{
	if (!sessions.empty())
	{
		for (auto& session : sessions)
		{
			delete session;
			session = nullptr;
		}
	}
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
	if (cs_attack_object_packet)
	{
		delete cs_attack_object_packet;
		cs_attack_object_packet = nullptr;
	}
	if (zone)
	{
		delete zone;
		zone = nullptr;
	}

	closesocket(server);
	WSACleanup();
}

void GameServer::Initialize()
{
	std::cout << "Network Initialization Start" << std::endl;

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

	std::cout << "Done" << std::endl;
}

void GameServer::InitializeNPC()
{
	std::cout << "NPC Initialization Start" << std::endl;

	LuaScript::MakeNewInstance(zone);

	for (int id = NPC_START; id < sessions.size(); ++id)
	{
		sessions[id]->SetState(STATE::INGAME);
		sessions[id]->GetMyObject()->CreateNewObject(random_x(dre), 270.0f, random_z(dre));
		sessions[id]->GetMyObject()->SetName("NPC-" + std::to_string(id));
		sessions[id]->SetID(id);

		zone->AddObject(id, sessions[id]);

		//dynamic_cast<NPC*>(sessions[id]->GetMyObject())->InitializeScript(id);
	}

	std::cout << "Done" << std::endl;
}

void GameServer::Accept()
{
	std::cout << "Client Acception Start" << std::endl;

	client_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);

	OVERLAPPEDEX accept_ex;
	accept_ex.type = COMPLETION::ACCEPT;
	accept_ex.wsa.buf = reinterpret_cast<char*>(client_socket);

	AcceptEx(server, client_socket, accept_ex.data, 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, 0, &accept_ex.over);
}

void GameServer::CreateThread()
{
	std::cout << "Create Worker Thread and AI Thread" << std::endl;

	for (int i = 0; i < MAX_USER; ++i)
	{
		worker_threads.emplace_back(&GameServer::ProcessWorkerThread, this);
	}

	ai_thread = std::thread{ &GameServer::ProcessAIThread, this };

	ai_thread.join();

	for (auto& thread : worker_threads)
	{
		thread.join();
	}
}

void GameServer::ProcessWorkerThread()
{
	DWORD bytes;
	ULONG_PTR id;
	BOOL ret;

	while (true)
	{
		ret = GetQueuedCompletionStatus(iocp, &bytes, &id, reinterpret_cast<LPOVERLAPPED*>(&over_ex), INFINITE);

		switch (over_ex->type)
		{
		case COMPLETION::ACCEPT:
		{
			AcceptClient();
		}
		break;
		case COMPLETION::RECV:
		{
			RecvData(id, bytes);
		}
		break;
		case COMPLETION::SEND:
		{
			SendData(id, bytes);
		}
		break;
		default:
			break;
		}
	}
}

void GameServer::AcceptClient()
{
	client_socket = reinterpret_cast<SOCKET>(over_ex->wsa.buf);

	if (int id{ NewPlayerID() }; id != -1)
	{
		sessions[id]->GetMyObject()->CreateNewObject(random_x(dre), 270, random_x(dre));
		//sessions[id]->GetMyObject()->CreateNewObject(random_x(dre), 270, random_x(dre));
		sessions[id]->SetID(id);
		sessions[id]->SetRemainSize(0);
		sessions[id]->SetSocket(client_socket);

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_socket), iocp, id, 0);

		sessions[id]->RecvData();

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

void GameServer::RecvData(ULONG_PTR id, DWORD bytes)
{
	if (bytes == 0)
	{
		Disconnect(id);
	}

	int remain_size{ static_cast<int>(bytes) + sessions[id]->GetRemainSize() };
	int packet_size{ over_ex->data[0] };

	packet = over_ex->data;

	for (; remain_size > 0 or packet_size <= remain_size; remain_size -= packet_size)
	{
		packet_size = packet[0];

		ProcessPacket(id);

		packet += packet_size;
	}

	sessions[id]->SetRemainSize(remain_size);

	if (remain_size > 0)
	{
		std::memcpy(over_ex->data, packet, remain_size);
	}

	sessions[id]->RecvData();
}

void GameServer::SendData(ULONG_PTR id, DWORD bytes)
{
	if (!bytes)
	{
		Disconnect(id);
	}

	over_ex->Reset();
}

void GameServer::ProcessAIThread()
{
}

void GameServer::ProcessPacket(int id)
{
	int packet_type{ packet[1] };

	switch (packet_type)
	{
	case CS::LOGIN:
	{
		ProcessLoginPacket(id);
	}
	break;
	case CS::MOVE_OBJECT:
	{
		ProcessMovePacket(id);
	}
	break;
	case CS::PLAYER_ATTACK:
	{
		ProcessPlayerAttackPacket(id);
	}
	break;
	case CS::ROTATE_OBJECT:
	{
		ProcessRotatePacket(id);
	}
	break;
	}
}

void GameServer::ProcessLoginPacket(int id)
{
	auto& player{ sessions[id] };

	cs_login_packet = reinterpret_cast<CS::PACKET::LOGIN*>(packet);

	XMFLOAT3 look{ cs_login_packet->look_x, cs_login_packet->look_y, cs_login_packet->look_z };
	XMFLOAT3 right{ cs_login_packet->right_x, cs_login_packet->right_y, cs_login_packet->right_z };
	XMFLOAT3 up{ cs_login_packet->up_x, cs_login_packet->up_y, cs_login_packet->up_z };

	if (player->GetState() == STATE::FREE)
	{
		return;
	}

	if (player->GetState() == STATE::INGAME)
	{
		Disconnect(id);
		return;
	}

	player->GetMyObject()->SetName(cs_login_packet->name);
	dynamic_cast<Player*>(player->GetMyObject())->SetLook(look);
	dynamic_cast<Player*>(player->GetMyObject())->SetRight(right);
	dynamic_cast<Player*>(player->GetMyObject())->SetUp(up);
	dynamic_cast<Player*>(player->GetMyObject())->SetPitch(cs_login_packet->pitch);
	dynamic_cast<Player*>(player->GetMyObject())->SetYaw(cs_login_packet->yaw);
	player->SendLoginPakcet();
	player->SetState(STATE::INGAME);

	zone->AddObject(id, player);

	for (int i = 0; i < MAX_USER; ++i)
	{
		auto& other{ sessions[i] };

		if (not other->IsMyID(id))
		{
			if (other->GetState() == STATE::INGAME)
			{
				POS disX{ player->GetMyObject()->GetX() - other->GetMyObject()->GetX() };
				POS disZ{ player->GetMyObject()->GetZ() - other->GetMyObject()->GetZ() };

				// 내가 다른 플레이어의 시야에 들어가면
				if (IsInSight(disX, disZ))
				{
					// 상대방 view list에 내 id 추가
					other->AddToViewList(id);
					// 상대방 클라이언트에 내 정보 전송
					other->SendAddObjectPacket(id, player->GetMyObject());
				}
			}
		}
	}

	for (auto& other : sessions)
	{
		if (not other->IsMyID(id))
		{
			if (other->GetState() == STATE::INGAME)
			{
				POS disX{ player->GetMyObject()->GetX() - other->GetMyObject()->GetX() };
				POS disZ{ player->GetMyObject()->GetZ() - other->GetMyObject()->GetZ() };

				// 상대가 내 시야에 들어오면
				if (IsInSight(disX, disZ))
				{
					// 내 view list에 상대방 id 추가
					player->AddToViewList(other->GetID());
					// 내 클라이언트에 상대방의 정보 전송
					player->SendAddObjectPacket(other->GetID(), other->GetMyObject());
				}
			}
		}
	}

	std::cout << "player[" << id << "] login" << std::endl;
}

void GameServer::ProcessMovePacket(int id)
{
	cs_move_object_packet = reinterpret_cast<CS::PACKET::MOVE_OBJECT*>(packet);

	zone->MovePlayer(id, cs_move_object_packet->direction);
}

void GameServer::ProcessRotatePacket(int id)
{
	cs_rotate_object_packet = reinterpret_cast<CS::PACKET::ROTATE_OBJECT*>(packet);

	dynamic_cast<Player*>(sessions[id]->GetMyObject())->Rotate(cs_rotate_object_packet->cx, cs_rotate_object_packet->cy);

	for (auto& session : sessions)
	{
		if (session->GetID() == id)
		{
			continue;
		}

		if (session->GetState() == STATE::INGAME)
		{
			session->SendRotateObjectPacket(id, session->GetMyObject());
		}
	}
}

void GameServer::ProcessPlayerAttackPacket(int id)
{
	cs_attack_object_packet = reinterpret_cast<CS::PACKET::PLAYER_ATTACK*>(packet);


}

int GameServer::NewPlayerID()
{
	if (active_users != MAX_USER)
	{
		int id{ active_users };

		// id가 사용 중이지 않으면 새로 할당
		if (sessions[id]->GetState() == STATE::FREE)
		{
			sessions[id]->SetState(STATE::ACCEPTED);

			// 액티브 유저 수 증가
			++active_users;

			return id;
		}
	}

	return -1;
}

int GameServer::NewRandomID()
{
	//if (active_users == MAX_USER)
	//{
	//	return -1;
	//}

	//int id{ random_id(dre) };

	//while (id_in_use[id])		// id가 사용 중인지 확인
	//{
	//	// id가 사용중이면 새로운 id 발급
	//	id = random_id(dre);
	//}

	//std::lock_guard<std::mutex> temp{ sessions[id]->state_lock };

	//// id가 사용 중인지 확인
	//if (sessions[id]->GetState() == STATE::FREE)
	//{
	//	// id가 사용 중이지 않으면 새로 할당
	//	sessions[id]->SetState(STATE::ACCEPTED);
	//	id_in_use[id] = true;

	//	// 액티브 유저 수 증가
	//	++active_users;

	//	return id;
	//}

	//sessions[id]->state_lock.unlock();
	return -1;
}

void GameServer::Disconnect(ULONG_PTR id)
{
	if (sessions[id]->GetState() == STATE::FREE)
	{
		return;
	}

	sessions[id]->Reset();
	--active_users;

	for (auto& object : sessions)
	{
		if (not object->IsMyID(id))
		{
			if (object->GetState() == STATE::INGAME)
			{
				object->SendRemoveObjectPacket(id);
			}
		}
	}

	std::cout << "session[" << id << "]" << "disconnected" << std::endl;
}

void GameServer::Run()
{
	std::cout << "Server Online" << std::endl;

	Initialize();
	InitializeNPC();
	Accept();
	CreateThread();
}
