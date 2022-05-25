#include "pch.hpp"
#include "protocol.hpp"
#include "OVERLAPPEDEX.hpp"
#include "Session.hpp"
#include "Zone.hpp"
#include "Player.hpp"
#include "GameServer.hpp"

std::uniform_int_distribution<int> random_id(0, MAX_USER - 1);
//std::uniform_real_distribution<float> random_x(0.0f, VAR_SIZE::WORLD_X);
//std::uniform_real_distribution<float> random_z(0.0f, VAR_SIZE::WORLD_Z);
std::uniform_real_distribution<float> random_x(0.0f, VAR_SIZE::WORLD_X);
std::uniform_real_distribution<float> random_z(0.0f, VAR_SIZE::WORLD_Z);

GameServer::GameServer() :
	server{ INVALID_SOCKET },
	server_key{ 99999 },
	iocp{ INVALID_HANDLE_VALUE },
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

	//for (int i = MAX_USER; i < NPC_NUM; ++i)
	//{
	//	sessions[i] = new Session{ new NPC{} };
	//}
}

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

void GameServer::BootServer()
{
	if (WSADATA wsa; WSAStartup(MAKEWORD(2, 2), &wsa) != NOERROR)
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

void GameServer::CreateThread()
{
	SOCKET client{ WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED) };
	OVERLAPPEDEX accept_ex;
	ZeroMemory(&accept_ex, sizeof(accept_ex));

	accept_ex.type = static_cast<char>(COMPLETION_TYPE::ACCEPT);
	accept_ex.wsa_buf.buf = reinterpret_cast<char*>(client);

	AcceptEx(server, client, accept_ex.data, 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, 0, &accept_ex.over);

	for (int i = 0; i < MAX_USER; ++i)
	{
		worker_threads.emplace_back(&GameServer::ProcessThread, this);
	}

	for (auto& thread : worker_threads)
	{
		thread.join();
	}
}

void GameServer::ProcessThread()
{
	DWORD bytes;
	ULONG_PTR key;
	BOOL ret;

	while (true)
	{
		ret = GetQueuedCompletionStatus(iocp, &bytes, &key, reinterpret_cast<LPOVERLAPPED*>(&over_ex), INFINITE);

		//if (!ret)
		//{
		//	if (over_ex && over_ex->type == static_cast<int>(COMPLETION_TYPE::ACCEPT))
		//	{
		//		std::cout << "Accept error" << std::endl;
		//	}
		//	else
		//	{
		//		std::cout << "GetQueuedCompletionStatus error on client[" << key << "]" << std::endl;
		//
		//		Disconnect(key);
		//
		//		if (over_ex && over_ex->type == static_cast<int>(COMPLETION_TYPE::SEND))
		//		{
		//			ZeroMemory(over_ex, sizeof(over_ex));
		//			over_ex = nullptr;
		//		}
		//
		//		continue;
		//	}
		//}

		switch (over_ex->type)
		{
		case static_cast<int>(COMPLETION_TYPE::ACCEPT):
		{
			AcceptClient();
		}
		break;
		case static_cast<int>(COMPLETION_TYPE::RECV):
		{
			if (ret == 0 or bytes == 0)
			{
				//std::cout << "GetQueuedCompletionStatus error on client[" << key << "]" << std::endl;
				DisplayError(std::wstring{ L"GetQueuedCompletionStatus error on client[" + std::to_wstring(key) + L"]" });

				if (server_key != key)
					Disconnect(key);

				continue;
			}
			RecvData(bytes, key);
		}
		break;
		case static_cast<int>(COMPLETION_TYPE::SEND):
		{
			SendData(bytes, key);
		}
		break;
		default:
			break;
		}
	}
}

void GameServer::AcceptClient()
{
	int id{ GetNewClientID() };
	SOCKET client{ reinterpret_cast<SOCKET>(over_ex->wsa_buf.buf) };

	if (id != -1)
	{
		sessions[id]->GetMyObject()->CreateNewObject(random_x(dre), 270, random_x(dre));
		//sessions[id]->GetMyObject()->CreateNewObject(random_x(dre), 270, random_x(dre));
		sessions[id]->SetID(id);
		sessions[id]->SetSocket(client);
		sessions[id]->SetRemainSize(0);

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(client), iocp, id, 0);

		sessions[id]->RecvData();

		client = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	}
	else
	{
		std::cout << "Max user exceeded\n";
	}

	ZeroMemory(&over_ex->over, sizeof(over_ex->over));
	over_ex->wsa_buf.buf = reinterpret_cast<char*>(client);

	AcceptEx(server, client, over_ex->data, 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, 0, &over_ex->over);
}

void GameServer::RecvData(DWORD bytes, ULONG_PTR id)
{
	if (!bytes)
	{
		//Disconnect(key);
		return;
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
		memcpy(over_ex->data, packet, remain_size);
	}

	sessions[id]->RecvData();
}

void GameServer::SendData(DWORD bytes, ULONG_PTR key)
{
	if (!bytes)
	{
		//Disconnect(key);
		return;
	}

	ZeroMemory(&over_ex, sizeof(over_ex));
	over_ex = nullptr;
}

void GameServer::ProcessPacket(int id)
{
	int packet_type{ packet[1] };

	switch (packet_type)
	{
	case CS::LOGIN:
	{
		ProcessLoginPacket(id, packet);
	}
	break;
	case CS::MOVE_OBJECT:
	{
		ProcessMovePacket(id, packet);
	}
	break;
	case CS::PLAYER_ATTACK:
	{
		ProcessPlayerAttackPacket(id, packet);
	}
	break;
	case CS::ROTATE_OBJECT:
	{
		ProcessRotatePacket(id, packet);
	}
	break;
	}
}

void GameServer::ProcessLoginPacket(int id, char* pack)
{
	cs_login_packet = reinterpret_cast<CS::PACKET::LOGIN*>(pack);

	DirectX::XMFLOAT3 look{ cs_login_packet->look_x, cs_login_packet->look_y, cs_login_packet->look_z };
	DirectX::XMFLOAT3 right{ cs_login_packet->right_x, cs_login_packet->right_y, cs_login_packet->right_z };
	DirectX::XMFLOAT3 up{ cs_login_packet->up_x, cs_login_packet->up_y, cs_login_packet->up_z };

	sessions[id]->state_lock.lock();

	if (sessions[id]->GetState() == SESSION_STATE::FREE)
	{
		sessions[id]->state_lock.unlock();
		return;
	}

	if (sessions[id]->GetState() == SESSION_STATE::INGAME)
	{
		sessions[id]->state_lock.unlock();
		Disconnect(id);

		return;
	}

	sessions[id]->GetMyObject()->SetName(cs_login_packet->name);
	dynamic_cast<Player*>(sessions[id]->GetMyObject())->SetLook(look);
	dynamic_cast<Player*>(sessions[id]->GetMyObject())->SetRight(right);
	dynamic_cast<Player*>(sessions[id]->GetMyObject())->SetUp(up);
	dynamic_cast<Player*>(sessions[id]->GetMyObject())->SetPitch(cs_login_packet->pitch);
	dynamic_cast<Player*>(sessions[id]->GetMyObject())->SetYaw(cs_login_packet->yaw);
	zone->AddObject(id, sessions[id]);
	zone->SetInSector(id);
	sessions[id]->SendLoginPakcet();
	sessions[id]->SetState(SESSION_STATE::INGAME);
	sessions[id]->state_lock.unlock();

	for (auto& client : sessions)
	{
		if (client->GetID() == id)
		{
			continue;
		}

		client->state_lock.lock();

		if (client->GetState() != SESSION_STATE::INGAME)
		{
			client->state_lock.unlock();
			continue;
		}

		// 두 객체 간의 거리가 플레이어의 가시거리보다 짧을 때 서로의 정보를 전송
		if (IsInSight(id, client->GetID()))
		{
			// view list에 플레이어 id 추가
			client->AddToViewList(id);
			// view list에 객체 id 추가
			sessions[id]->AddToViewList(client->GetID());

			// 모든 플레이어에게 새로 접속한 플레이어 정보 전송
			client->SendAddObjectPacket(id, sessions[id]->GetMyObject());

			// 나에게 접속해 있는 모든 플레이어의 정보 전송
			sessions[id]->SendAddObjectPacket(client->GetID(), client->GetMyObject());
		}
		client->state_lock.unlock();
	}

	std::cout << "player" << id << " login" << std::endl;
}

void GameServer::ProcessMovePacket(int id, char* pack)
{
	cs_move_object_packet = reinterpret_cast<CS::PACKET::MOVE_OBJECT*>(pack);

	zone->MoveObject(id, cs_move_object_packet->direction);

	//sessions[id]->GetMyObject()->Move(cs_move_object_packet->direction, look_x, look_z, right_x, right_z);

	//for (auto& client : sessions)
	//{
	//	client->state_lock.lock_shared();

	//	if (client->GetState() == SESSION_STATE::INGAME)
	//	{
	//		client->SendMoveObjectPacket(id, sessions[id]->GetMyObject());
	//	}

	//	client->state_lock.unlock_shared();
	//}
}

void GameServer::ProcessRotatePacket(int id, char* pack)
{
	cs_rotate_object_packet = reinterpret_cast<CS::PACKET::ROTATE_OBJECT*>(pack);

	dynamic_cast<Player*>(sessions[id]->GetMyObject())->Rotate(cs_rotate_object_packet->cx, cs_rotate_object_packet->cy);

	for (auto& session : sessions)
	{
		if (session->GetID() == id)
		{
			continue;
		}

		session->state_lock.lock_shared();

		if (session->GetState() == SESSION_STATE::INGAME)
		{
			session->SendRotateObjectPacket(id, session->GetMyObject());
		}
	}
}

void GameServer::ProcessPlayerAttackPacket(int id, char* pack)
{
	cs_attack_object_packet = reinterpret_cast<CS::PACKET::PLAYER_ATTACK*>(pack);


}

int GameServer::GetNewClientID()
{
	if (active_users == MAX_USER)
	{
		return -1;
	}

	int id{ active_users };

	sessions[id]->state_lock.lock();

	// id가 사용 중이지 않으면 새로 할당
	if (sessions[id]->GetState() == SESSION_STATE::FREE)
	{
		sessions[id]->SetState(SESSION_STATE::ACCEPTED);
		sessions[id]->state_lock.unlock();

		// 액티브 유저 수 증가
		++active_users;

		return id;
	}

	sessions[id]->state_lock.unlock();

	return -1;
}

int GameServer::GetNewNPCID()
{
	if (active_npcs == NPC_NUM)
	{
		return -1;
	}

	int id{ active_npcs + MAX_USER };

	sessions[id]->state_lock.lock();

	// id가 사용 중이지 않으면 새로 할당
	if (sessions[id]->GetState() == SESSION_STATE::FREE)
	{
		sessions[id]->SetState(SESSION_STATE::ACCEPTED);
		sessions[id]->state_lock.unlock();

		// 액티브 NPC 수 증가
		++active_npcs;

		return id;
	}

	sessions[id]->state_lock.unlock();

	return -1;
}

int GameServer::GetNewRandomID()
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
	//if (sessions[id]->GetState() == SESSION_STATE::FREE)
	//{
	//	// id가 사용 중이지 않으면 새로 할당
	//	sessions[id]->SetState(SESSION_STATE::ACCEPTED);
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
	sessions[id]->state_lock.lock();

	if (sessions[id]->GetState() == SESSION_STATE::FREE)
	{
		sessions[id]->state_lock.unlock();

		return;
	}

	//closesocket(sessions[id]->GetSocket());
	//sessions[id]->SetState(SESSION_STATE::FREE);
	sessions[id]->Reset();
	--active_users;

	sessions[id]->state_lock.unlock();

	SC::PACKET::REMOVE_OBJECT packet;

	for (auto& object : sessions)
	{
		if (object->GetID() == id)
		{
			continue;
		}

		object->state_lock.lock_shared();

		if (object->GetState() != SESSION_STATE::INGAME)
		{
			object->state_lock.unlock_shared();
			continue;
		}

		object->state_lock.unlock_shared();
		object->SendRemoveObjectPacket(id);
	}
}

void GameServer::Run()
{
	BootServer();
	CreateThread();
}
