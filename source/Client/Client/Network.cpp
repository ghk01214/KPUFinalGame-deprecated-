#include "pch.h"
#include "Network.h"
#include "../../Server/Server/protocol.hpp"

CNetwork::CNetwork(CGameFramework* instance) : server(INVALID_SOCKET), remain_size(0), game_instance(instance), over(nullptr), over_ex(nullptr), packet(nullptr)
{
	ZeroMemory(&recv_over, sizeof(recv_over));
	ZeroMemory(&send_over, sizeof(send_over));
}

CNetwork::~CNetwork()
{
	delete game_instance;
	game_instance = nullptr;

	closesocket(server);
	WSACleanup();
}

void CNetwork::CreateNetworkSocket()
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

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(server), iocp, server, 0);
}

void CNetwork::ConnectToServer()
{
	sockaddr_in server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	InetPton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);

	worker_thread = std::thread{ &ProcessThread };

	WSAConnect(server, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr), 0, 0, 0, 0);

	worker_thread.join();
}

void CNetwork::RecvData(SOCKET sock)
{
	DWORD flag{ 0 };
	ZeroMemory(&recv_over, sizeof(recv_over));

	recv_over.wsa_buf.buf = recv_over.data + remain_size;
	recv_over.wsa_buf.len = VAR_SIZE::DATA - remain_size;

	WSARecv(server, &recv_over.wsa_buf, 1, 0, &flag, &recv_over.over, nullptr);
}

void CNetwork::SendData(void* packet)
{
	send_over.SetPacket(reinterpret_cast<char*>(packet));

	WSASend(server, &send_over.wsa_buf, 1, 0, 0, &send_over.over, nullptr);
}

void CNetwork::ProcessThread()
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
			ErrorQuit(L"GetQueuedCompletionStatus error occured", WSAGetLastError());
		}

		switch (over_ex->type)
		{
		case static_cast<int>(COMPLETION_TYPE::RECV):
		{

		}
		break;
		case static_cast<int>(COMPLETION_TYPE::SEND):
		{

		}
		break;
		}
	}
}

void CNetwork::RecvPacket(DWORD bytes)
{
	int prev_size{ static_cast<int>(bytes) + remain_size };
	int packet_size{ over_ex->data[1] };

	packet = over_ex->data;

	for (int size = prev_size; size > 0 || packet_size <= size; size -= packet_size)
	{
		ProcessPacket();

		packet += packet_size;
	}

	remain_size = prev_size;

	if (prev_size > 0)
	{
		memcpy(over_ex->data, packet, prev_size);
	}

	// 패킷 포인터 초기화
	packet[0] = '\0';
	packet = nullptr;
}

void CNetwork::ProcessPacket()
{
	int packet_type{ packet[1] };
	
	switch (packet_type)
	{
	case SC::ADD_PLAYER:
	{
		//game_instance->AddPlayer();
	}
	break;
	case SC::MOVE_PLAYER:
	{
		//game_intsance->MovePlayer();
	}
	break;
	}
}
