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

	void SetPacket(char* packet);

public:
	OVERLAPPED over;
	WSABUF wsa_buf;
	char data[VAR_SIZE::DATA];
	char type;
};

class CClient
{
public:
	CClient();
	
	void RecvData();
	void SendData(void* packet);

	void SendLoginPakcet();
	void SendMovePlayerPacket(short plId, char type, CPlayer pl);

	void SetState(SESSION_STATE session) { state = session; }

	SESSION_STATE GetState() { return state; }
	SOCKET GetSocket() { return sock; }
	int GetID() const { return id; }
	CPlayer GetPlayer() { return player; }
	int GetRemainSize() { return remain_size; }

	void SetID(int ID) { id = ID; }
	void SetSocket(SOCKET soc) { sock = soc; }
	void SetRemainSize(int size) { remain_size = size; }

private:
	OVERLAPPEDEX recv_over;
	OVERLAPPEDEX send_over;

	SC::PACKET::LOGIN login_packet;
	SC::PACKET::MOVE_PLAYER pl_move_packet;

	SESSION_STATE state;
	SOCKET sock;
	int id;
	CPlayer player;

	int remain_size;
public:

	std::mutex mu;
};

//===========================================================================================

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

	void ProcessPacket(int id, char* pack);
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

	CS::PACKET::LOGIN login_packet;
	SC::PACKET::ADD_PLAYER add_player_packet;
};

#endif // !_NETWORK_FRAMEWORK_HPP_
