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

	void SetInSector(int id);
	int UpdateSector(Session* client);

	void AddObject(Session* client);
	void RemoveObject(int id);
	void MoveObject(int id, int direction);

	Session* GetSession(int id) { return objects[id]; }

private:
	float width;
	float height;

	int sector_num_x;
	int sector_num_z;

	c_map objects;
	std::shared_mutex zone_lock;
	
	std::vector<std::vector<Sector>> sector;
};

