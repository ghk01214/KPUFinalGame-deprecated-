#ifndef _CLIENT_HPP_
#define	_CLIENT_HPP_

#include "OVERLAPPEDEX.hpp"
#include "Object.hpp"

enum class SESSION_STATE
{
	FREE, ACCEPTED, INGAME
};

class Session
{
public:
	Session();
	Session(Object* obj);
	~Session();

	void Reset();
	void OnDestroy();

	void RecvData();
	void SendData(void* packet);

	void SendLoginPakcet();
	void SendMoveObjectPacket(int id, Object* pl);
	void SendAddObjectPacket(int id, Object* pl);
	void SendRemoveObjectPacket(int id);

	void AddToViewList(int id);
	void RemoveFromViewList(int id);

	SESSION_STATE GetState() const { return state; }
	SOCKET GetSocket() { return sock; }
	int GetID() const { return id; }
	Object* GetMyObject() { return object; }
	int GetRemainSize() { return remain_size; }
	int* GetSectorIndexX() { return &sector_index_x; }
	int* GetSectorIndexZ() { return &sector_index_z; }
	concurrency::concurrent_unordered_set<int>* GetViewList() { return &view_list; }

	void SetState(SESSION_STATE session) { state = session; }
	void SetID(int ID) { id = ID; }
	void SetSocket(SOCKET new_socket) { sock = new_socket; }
	void SetRemainSize(int size) { remain_size = size; }
	void SetSectorIndex(int x, int y) { sector_index_x = x; sector_index_z = y; }
	void SetNewObject(Object* new_obj) { object = new_obj; }

private:
	OVERLAPPEDEX recv_ex;
	OVERLAPPEDEX send_ex;

	SC::PACKET::LOGIN sc_login_packet;
	SC::PACKET::MOVE_OBJECT sc_move_object_packet;
	SC::PACKET::ADD_OBJECT sc_add_object_packet;
	SC::PACKET::REMOVE_OBJECT sc_remove_object_packet;

	SESSION_STATE state;
	SOCKET sock;
	int id;

	Object* object;
	concurrency::concurrent_unordered_set<int> view_list;
	std::shared_mutex view_lock;
	int sector_index_x, sector_index_z;

	int remain_size;
	DWORD flag;
public:
	std::shared_mutex state_lock;
};

#endif // !_CLIENT_HPP_
