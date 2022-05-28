#ifndef _CLIENT_HPP_
#define	_CLIENT_HPP_

#include "OVERLAPPEDEX.hpp"
#include "Object.hpp"

enum class STATE
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

	void RecvData();
	void SendData(void* packet);

	void SendLoginPakcet();
	void SendAddObjectPacket(int id, Object* pl);
	void SendRemoveObjectPacket(int id);
	void SendMoveObjectPacket(int id, Object* pl);
	void SendRotateObjectPacket(int id, Object*pl);

	void AddToViewList(int id);
	void RemoveFromViewList(int id);

	bool IsMyID(int comp_id) { return id == comp_id; }
	bool IsInMyViewList(int comp_id) { return view_list.find(comp_id) != view_list.end(); }

	const STATE GetState() const noexcept { return state.load(std::memory_order_seq_cst); }
	constexpr SOCKET GetSocket() const noexcept { return sock; }
	constexpr int GetID() const noexcept { return id; }
	Object* GetMyObject() { return object; }
	constexpr int GetRemainSize() const noexcept { return remain_size; }
	int* GetSectorIndexX() { return &sector_index_x; }
	int* GetSectorIndexZ() { return &sector_index_z; }

	void SetSocket(SOCKET new_socket) { sock = new_socket; }
	void SetID(int ID) { id = ID; }
	void SetState(STATE new_state) { state.store(new_state, std::memory_order_seq_cst); }
	void SetMyObject(Object* new_obj) { object = new_obj; }
	void SetRemainSize(int size) { remain_size = size; }
	void SetSectorIndex(int x, int z) { sector_index_x = x; sector_index_z = z; }

private:
	SOCKET sock;
	int id;
	std::atomic<STATE> state;

	OVERLAPPEDEX recv_ex;
	OVERLAPPEDEX send_ex;

	Object* object;

	SC::PACKET::LOGIN sc_login_packet;
	SC::PACKET::ADD_OBJECT sc_add_object_packet;
	SC::PACKET::REMOVE_OBJECT sc_remove_object_packet;
	SC::PACKET::MOVE_OBJECT sc_move_object_packet;
	SC::PACKET::ROTATE_OBJECT sc_rotate_object_packet;

	int sector_index_x;
	int sector_index_z;

	DWORD flag;
	int remain_size;
public:
	c_set view_list;
	std::shared_mutex view_lock;

	std::chrono::steady_clock::time_point move_time;
};

#endif // !_CLIENT_HPP_
