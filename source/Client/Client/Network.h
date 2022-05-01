#pragma once

#include "../../Server/Server/protocol.hpp"

enum class COMPLETION_TYPE
{
	RECV, SEND
};

class CGameFramework;

struct NetworkData
{
	OVERLAPPED over;
	WSABUF wsa;
	char data[VAR_SIZE::DATA];
};

class OVERLAPPEDEX
{
public:
	OVERLAPPEDEX() : type(static_cast<char>(COMPLETION_TYPE::RECV)), wsa_buf({ VAR_SIZE::DATA, data })
	{
		ZeroMemory(&over, sizeof(over));
	}

	void SetPacket(char* packet)
	{
		type = static_cast<char>(COMPLETION_TYPE::SEND);
		wsa_buf.buf = data;
		wsa_buf.len = static_cast<ULONG>(packet[0]);

		ZeroMemory(&over, sizeof(over));
		std::memcpy(data, packet, packet[0]);
	}

public:
	OVERLAPPED over;
	WSABUF wsa_buf;
	char data[VAR_SIZE::DATA];
	char type;
};

class CNetwork
{
public:
	CNetwork() = default;
	CNetwork(CGameFramework* game_inst);
	~CNetwork();

	void ConnectToServer();
	void ProcessThread();

	void RecvData();
	void SendData(void* pack);

	void RecvData(DWORD bytes);
	void SendData(DWORD bytes);

	void ProcessPacket();
	void ProcessLoginPacket();
	void ProcessMovePacket();

	void SendLoginPacket();
	void SendMovePlayerPacket(DWORD direction);

private:
	SOCKET server;
	HANDLE iocp;
	
	std::thread worker_thread;

	OVERLAPPED* over;
	OVERLAPPEDEX* over_ex;

	OVERLAPPEDEX recv_ex;
	OVERLAPPEDEX send_ex;

	SC::PACKET::LOGIN* sc_login_packet;
	SC::PACKET::MOVE_PLAYER* sc_move_player_packet;
	
	CS::PACKET::LOGIN* cs_login_packet;
	CS::PACKET::MOVE_PLAYER* cs_move_player_packet;

	char* packet;

	int remain_size;
	
	CGameFramework* game_instance;
};
