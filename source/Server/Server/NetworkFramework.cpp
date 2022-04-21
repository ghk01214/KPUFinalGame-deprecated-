#include "pch.hpp"
#include "NetworkFramework.hpp"

OVERLAPPEDEX::OVERLAPPEDEX() : type(static_cast<char>(COMPLETION_TYPE::RECV)), wsaBuf({ VAR_SIZE::DATA, data })
{
	ZeroMemory(&over, sizeof(over));
}

OVERLAPPEDEX::OVERLAPPEDEX(char* packet) : type(static_cast<char>(COMPLETION_TYPE::SEND)), wsaBuf({ static_cast<ULONG>(packet[0]), data})
{
	ZeroMemory(&over, sizeof(over));
	std::memcpy(data + 2, packet, packet[0]);
}

//===========================================================================================

CNetworkFramework::CNetworkFramework() : serverKey(9999)
{
}

CNetworkFramework::~CNetworkFramework()
{
	closesocket(server);
	WSACleanup();
}

bool CNetworkFramework::Initialize()
{
	WSADATA wsa;
	
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != NOERROR)
	{
		return false;
	}

	iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, NULL);
	server = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);

	sockaddr_in serverAddr;
	ZeroMemory(&serverAddr, sizeof(sockaddr_in));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(server), iocp, serverKey, 0);

	if (bind(server, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
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
	OVERLAPPEDEX acceptOver;
	ZeroMemory(&acceptOver, sizeof(acceptOver));

	acceptOver.type = static_cast<char>(COMPLETION_TYPE::ACCEPT);
	acceptOver.wsaBuf.buf = reinterpret_cast<char*>(client);

	AcceptEx(server, client, acceptOver.data, 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, 0, &acceptOver.over);

	for (int i = 0; i < MAX_USER; ++i)
	{
		workerThreads.emplace_back(&CNetworkFramework::ProcessThread, this);
	}

	for (auto& thread : workerThreads)
	{
		thread.join();
	}
}

void CNetworkFramework::ProcessThread()
{
	DWORD bytes;
	ULONG_PTR key;
	OVERLAPPED* over{ nullptr };
	OVERLAPPEDEX* overEX{ nullptr };
	BOOL ret;

	while (true)
	{
		ret = GetQueuedCompletionStatus(iocp, &bytes, &key, &over, INFINITE);
		overEX = reinterpret_cast<OVERLAPPEDEX*>(over);

		if (!ret)
		{
			if (overEX->type == static_cast<int>(COMPLETION_TYPE::ACCEPT))
			{
				std::cout << "Accept error" << std::endl;
			}
			else
			{
				std::cout << "GetQueuedCompletionStatus error on client[" << key << "]" << std::endl;

				DisconnectClient(key);

				if (overEX->type == static_cast<int>(COMPLETION_TYPE::SEND))
				{
					ZeroMemory(overEX, sizeof(overEX));
					overEX = nullptr;
				}

				continue;
			}
		}

		switch (overEX->type)
		{
		case static_cast<int>(COMPLETION_TYPE::ACCEPT):
		{
			int id{ GetNewClientID() };
			SOCKET client{ reinterpret_cast<SOCKET>(overEX->wsaBuf.buf) };
		}
		}
	}
}

void CNetworkFramework::ProcessPacket()
{
}

int CNetworkFramework::GetNewClientID()
{
	for (int i = 0; i < MAX_USER; ++i)
	{
		clients[i].inUse.lock();

		if (clients[i].GetState() == SESSION_STATE::FREE)
		{
			clients[i].SetState(SESSION_STATE::ACCEPTED);
			clients[i].inUse.unlock();

			return i;
		}

		clients[i].inUse.unlock();
	}

	return -1;
}

void CNetworkFramework::DisconnectClient(int id)
{
	SC::PACKET::MOVE_PLAYER packet;

	for (auto& player : clients)
	{
		if (player.GetState() == SESSION_STATE::INGAME || player.GetID() == id)
		{
			continue;
		}

		packet.id = id;
		packet.size = sizeof(packet);
		packet.type = SC::REMOVE_PLAYER;

		player.SendData(&packet);
	}

	closesocket(clients[id].GetSocket());
	clients[id].SetState(SESSION_STATE::FREE);
}


