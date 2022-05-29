#ifndef _GAME_SERVER_HPP_
#define _GAME_SERVER_HPP_

// INCLUDE
// OVERLAPPEDEX.hpp
// Object.hpp
#include "Session.hpp"

class Zone;

class GameServer
{
public:
	GameServer();
	~GameServer();
	
	void Run();
private:
	// 서버 복사 방지
	GameServer(const GameServer& other) = delete;
	GameServer& operator=(const GameServer& other) = delete;

	void Initialize();
	void InitializeNPC();
	void Accept();
	void CreateThread();

	// Main Thread
	void ProcessWorkerThread();
	void AcceptClient();
	void RecvData(ULONG_PTR id, DWORD bytes);
	void SendData(ULONG_PTR id, DWORD bytes);

	// AI Thread
	void ProcessAIThread();

	void ProcessPacket(int id);
	void ProcessLoginPacket(int id);
	void ProcessMovePacket(int id);
	void ProcessRotatePacket(int id);
	void ProcessPlayerAttackPacket(int id);

	int NewPlayerID();
	int NewRandomID();
	void Disconnect(ULONG_PTR id);

private:
	HANDLE iocp;
	SOCKET server;
	int server_key;

	SOCKET client_socket;
	std::array<Session*, MAX_USER/*+ NPC_NUM*/> sessions;

	OVERLAPPEDEX* over_ex;
	char* packet;

	CS::PACKET::LOGIN* cs_login_packet;
	CS::PACKET::MOVE_OBJECT* cs_move_object_packet;
	CS::PACKET::ROTATE_OBJECT* cs_rotate_object_packet;
	CS::PACKET::PLAYER_ATTACK* cs_attack_object_packet;

	std::vector<std::thread> worker_threads;
	std::thread ai_thread;

	Zone* zone;

	int active_users;
};
#endif // !_GAME_SERVER_HPP_
