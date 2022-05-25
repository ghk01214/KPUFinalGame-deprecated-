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

	void RecvData();
	void SendData(void* pack);

	void RecvData(DWORD bytes);
	void SendData(DWORD bytes);

	void ProcessPacket();
	void ProcessLoginPacket();
	void ProcessMovePacket();
	void ProcessAddObjectPacket();
	void ProcessRemoveObjectPacket();

	void SendLoginPacket();
	void SendMoveObjectPacket(DWORD direction);
	void SendRotateObjectPacket(float cx, float cy);
	void SendPlayerAttackPacket(int mode);

	int GetClientID() { return sc_login_packet->id; }

private:
	SOCKET server;
	HANDLE iocp;
	
	std::thread worker_thread;

	OVERLAPPEDEX* over_ex;

	OVERLAPPEDEX recv_ex;
	OVERLAPPEDEX send_ex;

	SC::PACKET::LOGIN* sc_login_packet;
	SC::PACKET::MOVE_OBJECT* sc_move_object_packet;
	SC::PACKET::ADD_OBJECT* sc_add_object_packet;
	SC::PACKET::REMOVE_OBJECT* sc_remove_object_packet;
	
	CS::PACKET::LOGIN* cs_login_packet;
	CS::PACKET::MOVE_OBJECT* cs_move_object_packet;
	CS::PACKET::ROTATE_OBJECT* cs_rotate_object_packet;
	CS::PACKET::PLAYER_ATTACK* cs_player_attack_packet;

	char* packet;

	int remain_size;
	
	CGameFramework* game_instance;
};
