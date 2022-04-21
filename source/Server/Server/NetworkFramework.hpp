#ifndef _NETWORK_FRAMEWORK_HPP_
#define _NETWORK_FRAMEWORK_HPP_

#include "protocol.hpp"
#include "Player.hpp"

enum class COMPLETION_TYPE
{
	ACCEPT, RECV, SEND
};

enum class SESSION_STATE
{
	FREE, ACCEPTED, INGAME
};

inline constexpr int MAX_USER{ 10 };

//===========================================================================================

class OVERLAPPEDEX
{
public:
	OVERLAPPEDEX();
	OVERLAPPEDEX(char* packet);

public:
	OVERLAPPED over;
	WSABUF wsaBuf;
	char data[VAR_SIZE::DATA];
	char type;
};

class CClient
{
public:
	CClient();
	
	void RecvData();
	void SendData(void* packet);

	void SendLoginPakcet(short plId, char type, CPlayer pl, char size = sizeof(SC::PACKET::LOGIN));
	void SendMovePlayerPacket(short plId, char type, CPlayer pl, char size = sizeof(SC::PACKET::MOVE_PLAYER));

	void SetState(SESSION_STATE session) { state = session; }

	SESSION_STATE GetState() { return state; }
	SOCKET GetSocket() { return sock; }
	int GetID() const { return id; }

private:
	OVERLAPPEDEX recvOver;
	OVERLAPPEDEX sendOver;

	SESSION_STATE state;
	SOCKET sock;
	int id;
	CPlayer player;

public:
	std::mutex inUse;
};

//===========================================================================================

class CNetworkFramework
{
public:
	CNetworkFramework();
	~CNetworkFramework();

	bool Initialize();
	void CreateThread();

	void ProcessThread();
	void ProcessPacket();

	int GetNewClientID();
	void DisconnectClient(int id);

private:
	SOCKET server;
	int serverKey;
	HANDLE iocp;

	std::vector<std::thread> workerThreads;
	std::array<CClient, MAX_USER> clients;
};

#endif // !_NETWORK_FRAMEWORK_HPP_
