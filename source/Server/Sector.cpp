#include "pch.h"
#include "Sector.h"

Sector::Sector() :
	lt_x{ VAR::WORLD_X_MIN },
	lt_z{ VAR::WORLD_Z_MAX }
{
}

Sector::Sector(POS x, POS z) :
	lt_x{ x },
	lt_z{ z }
{
}

Sector::~Sector()
{
}

Sector::Sector(const Sector& right) :
	lt_x{ right.lt_x },
	lt_z{ right.lt_z }
{
	std::memcpy(&objects, &right.objects, right.objects.size());
}

Sector& Sector::operator=(const Sector& right)
{
	if (this != &right)
	{
		lt_x = right.lt_x;
		lt_z = right.lt_z;

		std::memcpy(&objects, &right.objects, right.objects.size());
	}

	return *this;
}

Sector::Sector(Sector&& right) noexcept :
	lt_x{ right.lt_x },
	lt_z{ right.lt_z }
{
	right.lt_x = VAR::WORLD_X_MIN;
	right.lt_z = VAR::WORLD_Z_MAX;

	sector_lock.lock();
	right.sector_lock.lock();

	objects.swap(right.objects);
	right.objects.clear();

	right.sector_lock.unlock();
	sector_lock.unlock();
}

Sector& Sector::operator=(Sector&& right) noexcept
{
	if (this != &right)
	{
		lt_x = right.lt_x;
		lt_z = right.lt_z;

		right.lt_x = VAR::WORLD_X_MIN;
		right.lt_z = VAR::WORLD_Z_MAX;

		sector_lock.lock();
		right.sector_lock.lock();

		objects.swap(right.objects);
		right.objects.clear();

		right.sector_lock.unlock();
		sector_lock.unlock();
	}

	return *this;
}

void Sector::EnterSector(int id)
{
	objects.insert(id);
}

void Sector::LeaveSector(int id)
{
	sector_lock.lock();
	objects.unsafe_erase(id);
	sector_lock.unlock();
}

void Sector::MakeNewViewList(c_set* new_list, Session* client, c_map* others)
{
	if (not objects.empty())
	{
		for (auto& obj : objects)
		{
			if (not client->IsMyID(obj))
			{
				auto dis1{ client->GetX() - (*others)[obj]->GetX() };
				auto dis2{ client->GetZ() - (*others)[obj]->GetZ() };

				if (::IsInSight(dis1, dis2))
				{
					new_list->insert(obj);
				}
			}
		}
	}
}

bool Sector::OutOfSectorXMin(POS x)
{
	if (lt_x <= x)
		return false;

	return true;
}

bool Sector::OutOfSectorXMax(POS x)
{
	if (x < lt_x + SECTOR_RANGE)
		return false;

	return true;
}

bool Sector::OutOfSectorZMin(POS z)
{
	if (lt_z <= z)
		return false;

	return true;
}

bool Sector::OutOfSectorZMax(POS z)
{
	if (z < lt_z + SECTOR_RANGE)
		return false;

	return true;
}