#include "pch.hpp"
#include "protocol.hpp"
#include "Object.hpp"

Object::Object() :
	x{ 0 },
	y{ 0 },
	z{ 0 },
	max_speed{ 2.25f }
{
}

Object::Object(POS new_x, POS new_y, POS new_z) :
	x{ new_x },
	y{ new_y },
	z{ new_z },
	max_speed{ 5.0f }
{
}

void Object::CreateNewObject(POS new_x, POS new_y, POS new_z)
{
	x = new_x;
	y = new_y;
	z = new_z;
}

bool Object::IsInWorld()
{
	if (x < 0 and x > VAR_SIZE::WORLD_X)
		return false;
	if (z < 0 and z > VAR_SIZE::WORLD_Z)
		return false;

	return true;
}
