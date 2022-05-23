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

	void BootServer();
	void CreateThread();
	void Run();

	void ProcessThread();
	void AcceptClient();
	void RecvData(DWORD bytes, ULONG_PTR id);
	void SendData(DWORD bytes, ULONG_PTR key);

	void ProcessPacket(int id);
	void ProcessLoginPacket(int id, char* pack);
	void ProcessMovePacket(int id, char* pack);
	void ProcessPlayerAttackPacket(int id, char* pack);

	int GetNewClientID();
	int GetNewNPCID();
	int GetNewRandomID();
	void Disconnect(ULONG_PTR id);	

	inline bool IsInSight(int id1, int id2);

private:
	SOCKET server;
	int server_key;
	HANDLE iocp;

	std::vector<std::thread> worker_threads;
	std::array<Session*, MAX_USER/* + NPC_NUM*/> sessions;
	int active_users;
	int active_npcs;

	OVERLAPPEDEX* over_ex;
	char* packet;

	CS::PACKET::LOGIN* cs_login_packet;
	CS::PACKET::MOVE_OBJECT* cs_move_object_packet;
	CS::PACKET::PLAYER_ATTACK* cs_player_object_packet;

	Zone* zone;
};

inline bool GameServer::IsInSight(int id1, int id2)
{
	if (SIGHT_RANGE <= std::abs(sessions[id1]->GetMyObject()->GetX() - sessions[id2]->GetMyObject()->GetX()))
		return false;
	if (SIGHT_RANGE <= std::abs(sessions[id1]->GetMyObject()->GetY() - sessions[id2]->GetMyObject()->GetY()))
		return false;
	if (SIGHT_RANGE <= std::abs(sessions[id1]->GetMyObject()->GetZ() - sessions[id2]->GetMyObject()->GetZ()))
		return false;

	return true;
}

#endif // !_GAME_SERVER_HPP_
