#include "pch.h"
#include "GameFramework.h"
#include "Network.h"

std::unique_ptr<CNetwork> CNetwork::instance{ nullptr };

CNetwork::CNetwork(CGameFramework* game_inst) :
	server(INVALID_SOCKET),
	remain_size(0),
	//game_instance(std::make_shared<CGameFramework>(*game_inst)),
	game_instance(game_inst),
	recv_login_packet(nullptr),
	recv_add_player_packet(nullptr),
	recv_move_player_packet(nullptr),
	recv_remove_player_packet(nullptr)
{
	instance = std::make_unique<CNetwork>(*this);
	a_send = a_recv = 0;
	ZeroMemory(&recv.over, sizeof(recv.over));
	ZeroMemory(&send.over, sizeof(send.over));
}

CNetwork::~CNetwork()
{
	delete recv_login_packet;
	delete recv_add_player_packet;
	delete recv_move_player_packet;
	delete recv_remove_player_packet;

	recv_login_packet = nullptr;
	recv_add_player_packet = nullptr;
	recv_move_player_packet = nullptr;
	recv_remove_player_packet = nullptr;

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

	//while (true)
	//{
	//	game_instance->FrameAdvance();
	//}
	SendLoginPacket();
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

void CNetwork::SendLoginPacket()
{
	strcpy_s(send_login_packet.name, "Player");

	std::memcpy(send.data, &send_login_packet, sizeof(CS::PACKET::LOGIN));

	SendData();
}

void CNetwork::SendMovePlayerPacket(DWORD direct)
{
	/*send_move_player_packet.size = sizeof(CS::PACKET::MOVE_PLAYER);
	send_move_player_packet.type = CS::MOVE_PLAYER;*/
	send_move_player_packet.direction = static_cast<char>(direct);
	
	std::memcpy(send.data, &send_move_player_packet, sizeof(CS::PACKET::MOVE_PLAYER));

	SendData();
}

void CALLBACK CNetwork::RecvCallback(DWORD error, DWORD bytes, LPWSAOVERLAPPED over, DWORD flag)
{
	instance->ProcessPacket();

	instance->recv.data[0] = '\0';

	instance->RecvData();
	instance->SendData();
}

void CALLBACK CNetwork::SendCallback(DWORD error, DWORD bytes, LPWSAOVERLAPPED over, DWORD flag)
{
	++instance->a_send;
	instance->send.data[0] = '\0';
}

void CNetwork::ProcessPacket()
{
	++instance->a_recv;
	int packet_type{ recv.data[1] };

	switch (packet_type)
	{
	case SC::LOGIN:
	{
		// 로그인 패킷으로 캐스팅
		recv_login_packet = reinterpret_cast<SC::PACKET::LOGIN*>(recv.data);


	}
	break;
	case SC::ADD_PLAYER:
	{
		// 플레이어 추가 패킷으로 캐스팅
		recv_add_player_packet = reinterpret_cast<SC::PACKET::ADD_PLAYER*>(recv.data);

	}
	break;
	case SC::MOVE_PLAYER:
	{
		// 플레이어 이동 패킷으로 캐스팅
		recv_move_player_packet = reinterpret_cast<SC::PACKET::MOVE_PLAYER*>(recv.data);

		game_instance->GetPlayer()->Move(recv_move_player_packet->x, recv_move_player_packet->y, recv_move_player_packet->z);
	}
	break;
	case SC::REMOVE_PLAYER:
	{
		// 플레이어 제거 패킷으로 캐스팅
		recv_remove_player_packet = reinterpret_cast<SC::PACKET::REMOVE_PLAYER*>(recv.data);


	}
	break;
	default:
		break;
	}

	game_instance->GetPlayer()->Update(game_instance->GetTimer()->GetTimeElapsed());
}
