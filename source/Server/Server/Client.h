#ifndef _CLIENT_HPP_
#define	_CLIENT_HPP_

#include "OVERLAPPEDEX.hpp"
#include "Player.hpp"

enum class SESSION_STATE
{
	FREE, ACCEPTED, INGAME
};

class CClient
{
public:
	CClient();

	void RecvData();
	void SendData(void* packet);

	void SendLoginPakcet();
	void SendMovePlayerPacket(short plId, char type, CPlayer* pl);
	void SendAddPlayerPacket(int player_id, CClient* client);

	void SetState(SESSION_STATE session) { state = session; }

	SESSION_STATE GetState() const { return state; }
	SOCKET GetSocket() { return sock; }
	int GetID() const { return id; }
	CPlayer* GetPlayer() { return player; }
	int GetRemainSize() { return remain_size; }

	void SetID(int ID) { id = ID; }
	void SetSocket(SOCKET soc) { sock = soc; }
	void SetRemainSize(int size) { remain_size = size; }

private:
	OVERLAPPEDEX recv_ex;
	OVERLAPPEDEX send_ex;

	SC::PACKET::LOGIN sc_login_packet;
	SC::PACKET::MOVE_PLAYER sc_move_player_packet;
	SC::PACKET::ADD_PLAYER sc_add_player_packet;

	SESSION_STATE state;
	SOCKET sock;
	int id;
	CPlayer* player;

	int remain_size;
public:

	std::mutex mu;
};

#endif // !_CLIENT_HPP_
