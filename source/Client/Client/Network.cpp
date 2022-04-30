#include "pch.h"
#include "Network.h"
#include "../../Server/Server/protocol.hpp"

std::unique_ptr<CNetwork> CNetwork::instance{ nullptr };

CNetwork::CNetwork(CGameFramework* game_inst) : 
	server(INVALID_SOCKET),
	remain_size(0),
	game_instance(std::make_unique<CGameFramework>(game_inst)),
	login_packet(nullptr),
	add_player_packet(nullptr),
	move_player_packet(nullptr),
	remove_player_packet(nullptr)
{
	instance = std::make_unique<CNetwork>(this);

	ZeroMemory(&recv.over, sizeof(recv.over));
	ZeroMemory(&send.over, sizeof(send.over));
}

CNetwork::~CNetwork()
{
	delete login_packet;
	delete add_player_packet;
	delete move_player_packet;
	delete remove_player_packet;

	login_packet = nullptr;
	add_player_packet = nullptr;
	move_player_packet = nullptr;
	remove_player_packet = nullptr;

	closesocket(server);
	WSACleanup();
}

void CNetwork::ConnectToServer()
{
	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != NOERROR)
	{
		ErrorQuit(L"WSAStartup fuction error", WSAGetLastError());
	}

	server = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);

	sockaddr_in server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	InetPton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);

	WSAConnect(server, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr), 0, 0, 0, 0);
}

void CNetwork::Run()
{
	RecvData();
	SendData();

	while (true)
	{
		game_instance->FrameAdvance();
	}
}

void CNetwork::RecvData()
{
	recv.wsa.buf = recv.data;
	recv.wsa.len = VAR_SIZE::DATA;

	DWORD flag{ 0 };

	ZeroMemory(&recv.over, sizeof(recv.over));

	int ret{ WSARecv(server, &recv.wsa, 1, 0, &flag, &recv.over, RecvCallback) };

	if (ret != NOERROR)
	{
		int error{ WSAGetLastError() };

		if (error != WSA_IO_PENDING)
		{
			ErrorQuit(L"WSARecv 에러", error);
		}
	}
}

void CNetwork::SendData()
{
	send.wsa.buf = send.data;
	send.wsa.len = send.data[0];

	ZeroMemory(&send.over, sizeof(send.over));

	WSASend(server, &send.wsa, 1, 0, 0, &send.over, SendCallback);
}

void CALLBACK CNetwork::RecvCallback(DWORD error, DWORD bytes, LPWSAOVERLAPPED over, DWORD flag)
{
	instance->ProcessPacket();

	instance->RecvData();
	instance->SendData();
}

void CALLBACK CNetwork::SendCallback(DWORD error, DWORD bytes, LPWSAOVERLAPPED over, DWORD flag)
{
}

void CNetwork::ProcessPacket()
{
	int packet_type{ recv.data[1] };

	switch (packet_type)
	{
	case SC::LOGIN:
	{
		// 로그인 패킷으로 캐스팅
		login_packet = reinterpret_cast<SC::PACKET::LOGIN*>(recv.data);


	}
	break;
	case SC::ADD_PLAYER:
	{
		// 플레이어 추가 패킷으로 캐스팅
		add_player_packet = reinterpret_cast<SC::PACKET::ADD_PLAYER*>(recv.data);

	}
	break;
	case SC::MOVE_PLAYER:
	{
		// 플레이어 이동 패킷으로 캐스팅
		move_player_packet = reinterpret_cast<SC::PACKET::MOVE_PLAYER*>(recv.data);

		game_instance->GetPlayer()->Move();
	}
	break;
	case SC::REMOVE_PLAYER:
	{
		// 플레이어 제거 패킷으로 캐스팅
		remove_player_packet = reinterpret_cast<SC::PACKET::REMOVE_PLAYER*>(recv.data);


	}
	break;
	default:
		break;
	}
}
