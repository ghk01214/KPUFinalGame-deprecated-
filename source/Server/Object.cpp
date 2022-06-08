#include "pch.h"
#include "Object.h"

Object::Object() :
	x{ 0.0f },
	y{ 0.0f },
	z{ 0.0f }
{
	name.resize(20);
}

Object::Object(POS x, POS y, POS z) :
	x{ x },
	y{ y },
	z{ z }
{
	name.resize(20);
}

Object::~Object()
{
	name.clear();
	name.shrink_to_fit();
}

void Object::Reset()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;

	if (!name.empty())
	{
		name.clear();
	}
}

void Object::SetPos(POS new_x, POS new_y, POS new_z)
{
	x = new_x;
	y = new_y;
	z = new_z;
}
