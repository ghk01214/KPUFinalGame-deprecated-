#ifndef _NETWORK_FRAMEWORK_HPP_
#define _NETWORK_FRAMEWORK_HPP_

#include "OVERLAPPEDEX.hpp"
#include "Client.h"

class CNetworkFramework
{
public:
	CNetworkFramework();
	~CNetworkFramework();

	void OnCreate();
	void OnDestroy();

	void CreateThread();
	void ProcessThread();
	void AcceptClient();
	void RecvData(DWORD bytes, ULONG_PTR key);
	void SendData(DWORD bytes, ULONG_PTR key);

	void ProcessPacket(int id);
	void ProcessLoginPacket(int id, char* pack);
	void ProcessMovePacket(int id, char* pack);

	int GetNewClientID();
	void DisconnectClient(ULONG_PTR id);

private:
	SOCKET server;
	int server_key;
	HANDLE iocp;

	std::vector<std::thread> worker_threads;
	std::array<CClient, MAX_USER> clients;

	OVERLAPPED* over;
	OVERLAPPEDEX* over_ex;
	char* packet;

	SC::PACKET::ADD_PLAYER sc_add_player_packet;

	CS::PACKET::LOGIN* cs_login_packet;
	CS::PACKET::MOVE_PLAYER* cs_move_player_packet;
};

#endif // !_NETWORK_FRAMEWORK_HPP_
