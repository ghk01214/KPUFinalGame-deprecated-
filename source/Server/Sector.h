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

	void EnterSector(int id);
	void LeaveSector(int id);
	
	void MakeNewViewList(c_set* new_list, Session* client, c_map* others);

	bool OutOfSectorXR(POS x);
	bool OutOfSectorXL(POS x);
	bool OutOfSectorZF(POS z);
	bool OutOfSectorZB(POS z);

	POS GetLBX() { return lb_x; }
	POS GetLBZ() { return lb_z; }
	
private:
	c_set objects;
	std::shared_mutex sector_lock;

	POS lb_x, lb_z;
};

