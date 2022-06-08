#pragma once

#include "Session.h"
#include "Object.h"

class Sector
{
private:
	using c_map = concurrency::concurrent_unordered_map<int, Session*>;

public:
	Sector();
	Sector(POS x, POS z);
	~Sector();

	Sector(const Sector& right);
	Sector& operator=(const Sector& right);

	Sector(Sector&& right) noexcept;
	Sector& operator=(Sector&& right) noexcept;

	void EnterSector(ID id);
	void LeaveSector(ID id);
	
	void MakeNewViewList(c_set* new_list, Session* client, c_map* others);

	bool OutOfSectorXMax(POS x);
	bool OutOfSectorXMin(POS x);
	bool OutOfSectorZMax(POS z);
	bool OutOfSectorZMin(POS z);

	POS GetLBX() { return lt_x; }
	POS GetLBZ() { return lt_z; }
	
private:
	c_set objects;
	std::shared_mutex sector_lock;

	POS lt_x, lt_z;
};

