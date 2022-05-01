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

//class CNetwork
//{
//public:
//	CNetwork() = default;
//	CNetwork(CGameFramework* game_inst);
//	~CNetwork();
//
//	void ConnectToServer();
//	void Run();
//
//	void RecvData();
//	void SendData();
//
//	void SendLoginPacket();
//	void SendMovePlayerPacket(DWORD direct);
//
//	static void CALLBACK RecvCallback(DWORD error, DWORD bytes, LPWSAOVERLAPPED over, DWORD flag);
//	static void CALLBACK SendCallback(DWORD error, DWORD bytes, LPWSAOVERLAPPED over, DWORD flag);
//
//	void ProcessPacket();
//
//private:
//	SOCKET server;
//
//	NetworkData recv;
//	NetworkData send;
//
//	int remain_size;
//
//	SC::PACKET::LOGIN* recv_login_packet;
//	SC::PACKET::ADD_PLAYER* recv_add_player_packet;
//	SC::PACKET::MOVE_PLAYER* recv_move_player_packet;
//	SC::PACKET::REMOVE_PLAYER* recv_remove_player_packet;
//
//	CS::PACKET::LOGIN send_login_packet;
//	CS::PACKET::MOVE_PLAYER send_move_player_packet;
//
//	static std::unique_ptr<CNetwork> instance;
//	//std::shared_ptr<CGameFramework> game_instance;
//	CGameFramework* game_instance;
//
//	int a_send;
//	int a_recv;
//};

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
