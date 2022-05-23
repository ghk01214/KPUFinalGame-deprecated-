#include "pch.hpp"
#include "protocol.hpp"
#include "Object.hpp"

Object::Object() :
	x{ 0 },
	y{ 0 },
	z{ 0 },
	max_speed{ 5.0f }
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
