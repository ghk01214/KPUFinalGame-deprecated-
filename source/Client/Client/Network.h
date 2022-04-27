#pragma once

#include "OVERLAPPEDEX.h"

class CNetwork
{
public:
	CNetwork();
	~CNetwork();

	// 네트워크 통신 클래스 복사 생성 방지
	CNetwork(const CNetwork& other) = delete;
	CNetwork& operator=(const CNetwork& other) = delete;

	void CreateNetworkSocket();
	void RecvData(SOCKET sock);
	void SendData(void* packet);

private:
	HANDLE iocp;
	SOCKET server;

	OVERLAPPEDEX recv_over;
	OVERLAPPEDEX send_over;

	int remain_size;
};
