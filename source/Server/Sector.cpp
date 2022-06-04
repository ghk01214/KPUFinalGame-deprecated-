#include "pch.h"
#include "Sector.h"

Sector::Sector() :
	lb_x{ VAR::WORLD_XL },
	lb_z{ VAR::WORLD_ZB }
{
}

Sector::Sector(POS x, POS z) :
	lb_x{ x },
	lb_z{ z }
{
}

Sector::~Sector()
{
}

Sector::Sector(const Sector& right) :
	lb_x{ right.lb_x },
	lb_z{ right.lb_z }
{
	std::memcpy(&objects, &right.objects, right.objects.size());
}

Sector& Sector::operator=(const Sector& right)
{
	if (this != &right)
	{
		lb_x = right.lb_x;
		lb_z = right.lb_z;

		std::memcpy(&objects, &right.objects, right.objects.size());
	}

	return *this;
}

Sector::Sector(Sector&& right) noexcept :
	lb_x{ right.lb_x },
	lb_z{ right.lb_z }
{
	right.lb_x = VAR::WORLD_XL;
	right.lb_z = VAR::WORLD_ZB;

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
		lb_x = right.lb_x;
		lb_z = right.lb_z;

		right.lb_x = VAR::WORLD_XL;
		right.lb_z = VAR::WORLD_ZB;

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

bool Sector::OutOfSectorXL(POS x)
{
	if (lb_x <= x)
		return false;

	return true;
}

bool Sector::OutOfSectorZB(POS z)
{
	if (lb_z <= z)
		return false;

	return true;
}

bool Sector::OutOfSectorXR(POS x)
{
	if (x < lb_x + SECTOR_RANGE)
		return false;

	return true;
}

bool Sector::OutOfSectorZF(POS z)
{
	if (z < lb_z + SECTOR_RANGE)
		return false;

	return true;
}