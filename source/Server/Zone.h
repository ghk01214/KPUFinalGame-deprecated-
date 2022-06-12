#pragma once

#include "Sector.h"
#include "Session.h"

class Zone
{
private:
	using c_map = concurrency::concurrent_unordered_map<int, Session*>;

public:
	Zone();
	~Zone();

	void SetInSector(ID id, c_map* cont);
	void UpdateSector(Session* client);

	void AddPlayer(Session* client);
	void AddNPC(Session* npc);
	void RemovePlayer(ID id);

	void MovePlayer(ID id, int direction);
	void MoveNPC();
	void RotatePlayer(ID id, float cx, float cy);

	Session* GetSession(ID id) { return npcs[id]; }

private:
	float width;
	float height;

	int sector_num_x;
	int sector_num_z;

	c_map players;
	c_map npcs;
	std::shared_mutex zone_lock;

	std::vector<std::vector<Sector>> sector;
};

