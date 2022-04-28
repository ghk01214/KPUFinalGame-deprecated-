#pragma once

#include "OVERLAPPEDEX.h"
#include "GameFramework.h"

class CNetwork
{
public:
	CNetwork() = default;
	CNetwork(CGameFramework* instance);
	~CNetwork();

	// 네트워크 통신 클래스 복사 생성 방지
	CNetwork(const CNetwork& other) = delete;
	CNetwork& operator=(const CNetwork& other) = delete;

	void CreateNetworkSocket();
	void ConnectToServer();
	void RecvData(SOCKET sock);
	void SendData(void* packet);

	void ProcessThread();
	void RecvPacket(DWORD bytes);
	void ProcessPacket();

private:
	HANDLE iocp;
	SOCKET server;

	OVERLAPPED* over;
	OVERLAPPEDEX* over_ex;

	OVERLAPPEDEX recv_over;
	OVERLAPPEDEX send_over;

	char* packet;
	int remain_size;

	CGameFramework* game_instance;

	std::thread worker_thread;
};
