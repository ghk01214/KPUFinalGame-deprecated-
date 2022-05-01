#pragma once

#include "../../Server/Server/protocol.hpp"

class CGameFramework;

struct NetworkData
{
	OVERLAPPED over;
	WSABUF wsa;
	char data[VAR_SIZE::DATA];
};

class CNetwork
{
public:
	CNetwork() = default;
	CNetwork(CGameFramework* game_inst);
	~CNetwork();

	void ConnectToServer();
	void Run();

	void RecvData();
	void SendData();

	void SendLoginPacket();
	void SendMovePlayerPacket(DWORD direct);

	static void CALLBACK RecvCallback(DWORD error, DWORD bytes, LPWSAOVERLAPPED over, DWORD flag);
	static void CALLBACK SendCallback(DWORD error, DWORD bytes, LPWSAOVERLAPPED over, DWORD flag);

	void ProcessPacket();

private:
	SOCKET server;

	NetworkData recv;
	NetworkData send;

	int remain_size;
	
	SC::PACKET::LOGIN* recv_login_packet;
	SC::PACKET::ADD_PLAYER* recv_add_player_packet;
	SC::PACKET::MOVE_PLAYER* recv_move_player_packet;
	SC::PACKET::REMOVE_PLAYER* recv_remove_player_packet;

	CS::PACKET::LOGIN send_login_packet;
	CS::PACKET::MOVE_PLAYER send_move_player_packet;

	static std::unique_ptr<CNetwork> instance;
	//std::shared_ptr<CGameFramework> game_instance;
	CGameFramework* game_instance;

	int a_send;
	int a_recv;
};
