#ifndef _ZONE_HPP_
#define _ZONE_HPP_

// INCLUDE Session.hpp
#include "Sector.hpp"

class Zone
{
public:
	Zone();
	~Zone();

	void SetInSector(int id);
	void UpdateSector(Session* session);

	void AddObject(int id, Session* session);
	void MoveObject(int id, int direction);

private:
	float zone_width;
	float zone_height;

	int sector_num_x;
	int sector_num_z;

	concurrency::concurrent_unordered_map<int, Session*> objects;
	std::array<std::array<Sector, static_cast<int>(VAR_SIZE::WORLD_Z) / SECTOR_RANGE + 1>, static_cast<int>(VAR_SIZE::WORLD_X) / SECTOR_RANGE + 1> sector;
};

#endif // !_ZONE_HPP_
