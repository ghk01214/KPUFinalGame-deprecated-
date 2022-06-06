#pragma once

#include "Sector.h"
#include "Session.h"

class Zone
{
private:
	using c_map = concurrency::concurrent_unordered_map<ID, Session*>;

public:
	Zone();
	~Zone();

	void SetInSector(ID id);
	void UpdateSector(Session* client);

	void AddObject(Session* client);
	void RemoveObject(ID id);
	void MoveObject(ID id, int direction);
	void RotateObject(ID id, float cx, float cy);

	Session* GetSession(ID id) { return objects[id]; }

private:
	float width;
	float height;

	int sector_num_x;
	int sector_num_z;

	c_map objects;
	std::shared_mutex zone_lock;
	
	std::vector<std::vector<Sector>> sector;
};

