#pragma once

#include "../../Server/Server/protocol.hpp"

enum class COMPLETION_TYPE
{
	RECV, SEND
};

class CGameFramework;

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
