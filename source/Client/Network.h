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

	void RecvData();
	void SendData(void* pack);

	void RecvData(DWORD bytes);
	void SendData(DWORD bytes);

	void ProcessPacket();
	void ProcessLoginPacket();
	void ProcessAddObjectPacket();
	void ProcessMovePacket();
	void ProcessRemoveObjectPacket();

	void SendLoginPacket();
	void SendMoveObjectPacket(DWORD direction);
	void SendRotateObjectPacket(float cx, float cy);
	void SendPlayerAttackPacket(int mode);

	int GetClientID() { return sc_login->id; }

private:
	SOCKET server;
	HANDLE iocp;
	int key;
	
	std::thread worker_thread;

	OVERLAPPEDEX* over_ex;

	OVERLAPPEDEX recv_ex;
	OVERLAPPEDEX send_ex;

	SC::P::LOGIN* sc_login;
	SC::P::ADD_OBJ* sc_add_object;
	SC::P::MOVE_OBJ* sc_move_object;
	SC::P::ROTATE_OBJ* sc_rotate_object;
	SC::P::DELETE_OBJ* sc_remove_object;
	
	CS::P::LOGIN* cs_login;
	CS::P::MOVE_OBJ* cs_move_object;
	CS::P::ROTATE_OBJ* cs_rotate_object;
	CS::P::PLAYER_ATTACK* cs_player_attack;

	char* packet;

	int remain_size;
	
	CGameFramework* game_instance;
};
