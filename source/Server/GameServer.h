#pragma once

#include "OVERLAPPEDEX.h"
#include "Session.h"
#include "Zone.h"

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

	// 서버 초기화
	void Initialize();
	void InitializeNPC();
	void Accept();
	void CreateThread();

	// Main Thread
	void WorkerThread();
	void AcceptClient(OVERLAPPEDEX* over_ex);
	void Recv(ULONG_PTR id, DWORD bytes, OVERLAPPEDEX* over_ex);
	void Send(ULONG_PTR id, DWORD bytes, OVERLAPPEDEX* over_ex);

	int NewPlayerID();
	int NewRandomID();
	void Disconnect(ULONG_PTR id);

	void ProcessPacket(ULONG_PTR id);
	void Login(ULONG_PTR id);
	void Move(ULONG_PTR id);
	void Rotate(ULONG_PTR id);
	void PlayerAttack(ULONG_PTR id);

	// AI Thread
	void AIThread();

private:
	HANDLE iocp;
	SOCKET server;
	ID server_key;

	std::array<Session*, VAR::MAX_USER/* + VAR::NPC_NUM*/> clients;

	std::vector<std::thread> worker_threads;
	std::thread ai_thread;

	CS::P::LOGIN* cs_login;
	CS::P::MOVE_OBJ* cs_move;
	CS::P::ROTATE_OBJ* cs_rotate;
	CS::P::PLAYER_ATTACK* cs_player_attack;

	char* packet;

	Zone* zone;
};

