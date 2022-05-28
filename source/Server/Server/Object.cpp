#include "pch.hpp"
#include "protocol.hpp"
#include "Object.hpp"

Object::Object() :
	x{ 0 },
	y{ 0 },
	z{ 0 },
	max_speed{ 2.25f }
{
	name.reserve(VAR::NAME);
}

Object::Object(POS new_x, POS new_y, POS new_z) :
	x{ new_x },
	y{ new_y },
	z{ new_z },
	max_speed{ 5.0f }
{
	name.reserve(VAR::NAME);
}

void Object::CreateNewObject(POS new_x, POS new_y, POS new_z)
{
	x = new_x;
	y = new_y;
	z = new_z;
}

void Object::Reset()
{
	x = 0;
	y = 0;

	if (!name.empty())
	{
		name.clear();
	}
}
