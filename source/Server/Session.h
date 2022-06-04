#pragma once

#include "OVERLAPPEDEX.h"
#include "Object.h"

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

	void Recv();
	void Send(void* pack);

	void SendLogin();
	void SendAdd(Session* client);
	void SendDelete(int target_id);
	void SendMove(Session* client, int time = 0);
	void SendRotate(Session* client);
	void SendPlayerAttack(Session* client);

	bool IsMyID(int comp_id) { return id == comp_id; }
	//bool IsPlayerID() { return id < MAX_USER; }
	//bool IsNPCID() { return id >= NPC_START; }

	void AddToViewList(int id);
	void DeleteFromViewList(int id);
	bool IsInViewList(int id) { return view_list.find(id) != view_list.end(); }

	void Move(int direction) { object->Move(direction); }
	void Rotate(float cx, float cy) { object->Rotate(cx, cy); }

	const STATE GetState() const noexcept { return state.load(std::memory_order_seq_cst); }
	constexpr int GetId() const noexcept { return id; }
	Object* GetMyObject()  { return object; }
	constexpr POS GetX() const noexcept { return object->GetX(); }
	constexpr POS GetY() const noexcept { return object->GetY(); }
	constexpr POS GetZ() const noexcept { return object->GetZ(); }
	constexpr const char* GetName() const noexcept { return object->GetName(); }

	void SetSocket(SOCKET client) { sock = client; }
	void SetState(STATE new_state) { state.store(new_state, std::memory_order_seq_cst); }
	void SetID(int new_id) { id = new_id; }
	void SetObjectName(const char* name) { object->SetName(name); }
	void SetObjectName(std::string_view name) { object->SetName(name); }
	void SetObjectPos(POS x, POS y, POS z) { object->SetPos(x, y, z); }
	void SetSectorIndex(int x, int z) { sector_index_x = x; sector_index_z = z; }

private:
	OVERLAPPEDEX recv_ex;
	OVERLAPPEDEX send_ex;
	std::atomic<STATE> state;
	SOCKET sock;

	int id;
	Object* object;

	SC::P::LOGIN sc_login;
	SC::P::ADD_OBJ sc_add_obj;
	SC::P::DELETE_OBJ sc_delete_obj;
	SC::P::MOVE_OBJ sc_move_obj;
	SC::P::ROTATE_OBJ sc_rotate_obj;
	SC::P::PLAYER_ATTACK sc_player_attack;

	std::chrono::steady_clock::time_point next_move_time;

	DWORD flag;
public:
	concurrency::concurrent_unordered_set<int> view_list;
	std::shared_mutex view_lock;

	int sector_index_x, sector_index_z;

	int remain;
};

