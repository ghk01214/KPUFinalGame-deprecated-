#pragma once

#include "GameFramework.h"

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

	// 네트워크 통신 클래스 복사 생성 방지
	CNetwork(const CNetwork& other) = delete;
	CNetwork& operator=(const CNetwork& other) = delete;

	void ConnectToServer();
	void Run();

	void RecvData();
	void SendData();

	static void CALLBACK RecvCallback(DWORD error, DWORD bytes, LPWSAOVERLAPPED over, DWORD flag);
	static void CALLBACK SendCallback(DWORD error, DWORD bytes, LPWSAOVERLAPPED over, DWORD flag);

	void ProcessPacket();

private:
	SOCKET server;

	NetworkData recv;
	NetworkData send;

	int remain_size;
	
	SC::PACKET::LOGIN* login_packet;
	SC::PACKET::ADD_PLAYER* add_player_packet;
	SC::PACKET::MOVE_PLAYER* move_player_packet;
	SC::PACKET::REMOVE_PLAYER* remove_player_packet;

	static std::unique_ptr<CNetwork> instance;
	std::unique_ptr<CGameFramework> game_instance;
};
