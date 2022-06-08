#pragma once

#include "OVERLAPPEDEX.h"

class CGameFramework;

class CNetwork
{
public:
	CNetwork() = default;
	CNetwork(CGameFramework* game_inst);
	~CNetwork();

	void ConnectToServer();
	void ProcessThread();
	void EndThread();

	void Recv();
	void Send(void* pack);

	void Recv(DWORD bytes, OVERLAPPEDEX* over_ex);
	void Send(DWORD bytes, OVERLAPPEDEX* over_ex);

	void ProcessPacket();
	void Login();
	void AddObject();
	void RemoveObject();
	void MoveObject();
	void RotateObject();

	void SendLogin();
	void SendMoveObject(DWORD direction);
	void SendRotateObject(float cx, float cy);
	void SendPlayerAttack(int mode);

private:
	SOCKET server;
	HANDLE iocp;
	int key;
	
	std::thread worker_thread;

	OVERLAPPEDEX recv_ex;
	OVERLAPPEDEX send_ex;

	SC::P::LOGIN* sc_login;
	SC::P::ADD_OBJ* sc_add_object;
	SC::P::MOVE_OBJ* sc_move_object;
	SC::P::ROTATE_OBJ* sc_rotate_object;
	SC::P::REMOVE_OBJ* sc_remove_object;
	
	CS::P::LOGIN cs_login;
	CS::P::MOVE_OBJ cs_move_object;
	CS::P::ROTATE_OBJ cs_rotate_object;
	CS::P::PLAYER_ATTACK cs_player_attack;

	char* packet;

	int remain_size;
	
	CGameFramework* game_instance;
};
