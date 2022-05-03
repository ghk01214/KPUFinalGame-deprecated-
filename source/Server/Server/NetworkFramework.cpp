#include "pch.hpp"
#include "protocol.hpp"
#include "NetworkFramework.hpp"

std::uniform_int_distribution<int> random_id(0, MAX_USER - 1);
std::uniform_real_distribution<float> urd_x(0.0f, VAR_SIZE::WORLD_X);
std::uniform_real_distribution<float> urd_z(0.0f, VAR_SIZE::WORLD_Z);

CNetworkFramework::CNetworkFramework() :
	server_key(9999),
	over(nullptr),
	over_ex(nullptr),
	packet(nullptr),
	active_users(0)
{
	id_in_use.fill(false);
}

CNetworkFramework::~CNetworkFramework()
{
	if (packet)
	{
		delete packet;
		packet = nullptr;
	}
	if (over)
	{
		delete over;
		over = nullptr;
	}
	if (over_ex)
	{
		delete over_ex;
		over_ex = nullptr;
	}

	closesocket(server);
	WSACleanup();
}

void CNetworkFramework::BootServer()
{
	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != NOERROR)
	{
		ErrorQuit(L"WSAStartup fuction error", WSAGetLastError());
	}

	if (iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, NULL);
		iocp == nullptr)
	{
		ErrorQuit(L"IOCP Handle Creation Failed", WSAGetLastError());
	}

	server = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);

	sockaddr_in server_addr;
	ZeroMemory(&server_addr, sizeof(sockaddr_in));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(server), iocp, server, 0);

	if (bind(server, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR)
	{
		ErrorQuit(L"bind function error", WSAGetLastError());
	}

	if (listen(server, SOMAXCONN) == SOCKET_ERROR)
	{
		ErrorQuit(L"listen function error", WSAGetLastError());
	}
}

void CNetworkFramework::CreateThread()
{
	SOCKET client{ WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED) };
	OVERLAPPEDEX accept_ex;
	ZeroMemory(&accept_ex, sizeof(accept_ex));

	accept_ex.type = static_cast<char>(COMPLETION_TYPE::ACCEPT);
	accept_ex.wsa_buf.buf = reinterpret_cast<char*>(client);

	AcceptEx(server, client, accept_ex.data, 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, 0, &accept_ex.over);

	for (int i = 0; i < MAX_USER; ++i)
	{
		worker_threads.emplace_back(&CNetworkFramework::ProcessThread, this);
	}

	for (auto& thread : worker_threads)
	{
		thread.join();
	}
}

void CNetworkFramework::ProcessThread()
{
	DWORD bytes;
	ULONG_PTR key;
	BOOL ret;

	while (true)
	{
		ret = GetQueuedCompletionStatus(iocp, &bytes, &key, &over, INFINITE);
		over_ex = reinterpret_cast<OVERLAPPEDEX*>(over);

		if (!ret)
		{
			if (over_ex->type == static_cast<int>(COMPLETION_TYPE::ACCEPT))
			{
				std::cout << "Accept error" << std::endl;
			}
			else
			{
				std::cout << "GetQueuedCompletionStatus error on client[" << key << "]" << std::endl;

				DisconnectClient(key);

				if (over_ex->type == static_cast<int>(COMPLETION_TYPE::SEND))
				{
					ZeroMemory(over_ex, sizeof(over_ex));
					over_ex = nullptr;
				}

				continue;
			}
		}

		switch (over_ex->type)
		{
		case static_cast<int>(COMPLETION_TYPE::ACCEPT):
		{
			AcceptClient();
		}
		break;
		case static_cast<int>(COMPLETION_TYPE::RECV):
		{
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

void CNetworkFramework::AcceptClient()
{
	int id{ GetNewClientID() };
	SOCKET client{ reinterpret_cast<SOCKET>(over_ex->wsa_buf.buf) };

	if (id != -1)
	{
		clients[id].GetPlayer()->SetPosX(1200);
		clients[id].GetPlayer()->SetPosY(500);
		clients[id].GetPlayer()->SetPosZ(1100);
		clients[id].SetID(id);
		clients[id].GetPlayer()->SetName(0);
		clients[id].SetSocket(client);
		clients[id].SetRemainSize(0);

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(client), iocp, id, 0);

		clients[id].RecvData();

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

void CNetworkFramework::RecvData(DWORD bytes, ULONG_PTR key)
{
	if (!bytes)
	{
		DisconnectClient(key);
	}

	int remain_size{ static_cast<int>(bytes) + clients[key].GetRemainSize() };
	int packet_size{ over_ex->data[0] };

	packet = over_ex->data;

	//while (remain_size > 0)
	//{
	//	int packetSize{ packet[0] };
	//	if (packetSize <= remain_size)
	//	{
	//		ProcessPacket(key, packet);
	//		packet += packetSize;
	//		remain_size -= packetSize;
	//	}
	//	else
	//		break;
	//}

	for (; remain_size > 0 or packet_size <= remain_size; remain_size -= packet_size)
	{
		ProcessPacket(key);

		packet += packet_size;
	}

	clients[key].SetRemainSize(remain_size);

	if (remain_size > 0)
	{
		memcpy(over_ex->data, packet, remain_size);
	}

	clients[key].RecvData();

	// 임시 패킷 초기화
	packet[0] = '\0';
	packet = nullptr;
}

void CNetworkFramework::SendData(DWORD bytes, ULONG_PTR key)
{
	if (!bytes)
	{
		DisconnectClient(key);
	}

	ZeroMemory(&over_ex, sizeof(over_ex));
	over_ex = nullptr;
}

void CNetworkFramework::ProcessPacket(int id)
{
	int packet_type{ packet[1] };

	switch (packet_type)
	{
	case CS::LOGIN:
	{
		ProcessLoginPacket(id, packet);
	}
	break;
	case CS::MOVE_PLAYER:
	{
		ProcessMovePacket(id, packet);
	}
	break;
	case CS::PLAYER_ATTACK:
	{
		ProcessPlayerAttackPacket(id, packet);
	}
	break;
	}
}

void CNetworkFramework::ProcessLoginPacket(int id, char* pack)
{
	cs_login_packet = reinterpret_cast<CS::PACKET::LOGIN*>(pack);

	clients[id].mu.lock();

	if (clients[id].GetState() == SESSION_STATE::FREE)
	{
		clients[id].mu.unlock();
		return;
	}

	if (clients[id].GetState() == SESSION_STATE::INGAME)
	{
		clients[id].mu.unlock();
		DisconnectClient(id);

		return;
	}

	clients[id].GetPlayer()->SetName(cs_login_packet->name);
	clients[id].SendLoginPakcet();
	clients[id].SetState(SESSION_STATE::INGAME);
	clients[id].mu.unlock();

	for (auto& client : clients)
	{
		if (client.GetID() == id)
		{
			continue;
		}

		std::lock_guard<std::mutex> a{ client.mu };

		if (client.GetState() != SESSION_STATE::INGAME)
		{
			continue;
		}

		// 모든 플레이어에게 새로 접속한 플레이어 정보 전송
		//sc_add_player_packet.id = id;
		//strcpy_s(sc_add_player_packet.name, clients[id].GetPlayer()->GetName());
		//sc_add_player_packet.x = clients[id].GetPlayer()->GetPosX();
		//sc_add_player_packet.y = clients[id].GetPlayer()->GetPosY();
		//client.SendData(&sc_add_player_packet);

		clients[id].SendAddPlayerPacket(id, &client);

		// 나에게 접속해 있는 모든 플레이어의 정보 전송
		//sc_add_player_packet.id = client.GetID();
		//strcpy_s(sc_add_player_packet.name, client.GetPlayer()->GetName());
		//sc_add_player_packet.x = client.GetPlayer()->GetPosX();
		//sc_add_player_packet.y = client.GetPlayer()->GetPosY();
		//clients[id].SendData(&sc_add_player_packet);

		client.SendAddPlayerPacket(client.GetID(), &clients[id]);
	}

	std::cout << "player" << id << " login" << std::endl;
}

void CNetworkFramework::ProcessMovePacket(int id, char* pack)
{
	cs_move_player_packet = reinterpret_cast<CS::PACKET::MOVE_PLAYER*>(pack);

	float look_x{ cs_move_player_packet->look_x };
	float look_z{ cs_move_player_packet->look_z };
	float right_x{ cs_move_player_packet->right_x };
	float right_z{ cs_move_player_packet->right_z };

	clients[id].GetPlayer()->Move(cs_move_player_packet->direction, look_x, look_z, right_x, right_z);

	for (auto& client : clients)
	{
		std::lock_guard<std::mutex> a{ client.mu };

		if (client.GetState() == SESSION_STATE::INGAME)
		{
			client.SendMovePlayerPacket(id, client.GetPlayer());
		}
	}
}

void CNetworkFramework::ProcessRemovePlayerPacket(int id, char* pack)
{
}

void CNetworkFramework::ProcessPlayerAttackPacket(int id, char* pack)
{
	cs_player_attack_packet = reinterpret_cast<CS::PACKET::PLAYER_ATTACK*>(pack);


}

int CNetworkFramework::GetNewClientID()
{
	if (active_users == MAX_USER)
	{
		return -1;
	}

	int id{ random_id(dre) };

	while (id_in_use[id])		// id가 사용 중인지 확인
	{
		// id가 사용중이면 새로운 id 발급
		id = random_id(dre);
	}

	std::lock_guard<std::mutex> temp{ clients[id].mu };

	// id가 사용 중인지 확인
	if (clients[id].GetState() == SESSION_STATE::FREE)
	{
		// id가 사용 중이지 않으면 새로 할당
		clients[id].SetState(SESSION_STATE::ACCEPTED);
		id_in_use[id] = true;

		// 액티브 유저 수 증가
		++active_users;

		return id;
	}
}

void CNetworkFramework::DisconnectClient(ULONG_PTR id)
{
	clients[id].mu.lock();

	if (clients[id].GetState() == SESSION_STATE::FREE)
	{
		clients[id].mu.unlock();

		return;
	}

	closesocket(clients[id].GetSocket());
	clients[id].SetState(SESSION_STATE::FREE);
	--active_users;

	clients[id].mu.unlock();

	SC::PACKET::REMOVE_PLAYER packet;

	for (auto& player : clients)
	{
		if (player.GetID() == id)
		{
			continue;
		}

		std::lock_guard<std::mutex> a{ player.mu };

		if (player.GetState() == SESSION_STATE::INGAME)
		{
			continue;
		}

		player.SendRemovePlayerPacket(id);
	}
}
