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
	void MovePlayer(int id, int direction);

	Session* GetSession(int id) { return objects[id]; }

private:
	float width;
	float height;

	int sector_num_x;
	int sector_num_z;

	c_map objects;
	std::vector<std::vector<Sector>> sector;
};

#endif // !_ZONE_HPP_
