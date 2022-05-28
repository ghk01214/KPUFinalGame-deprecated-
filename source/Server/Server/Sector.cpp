#include "pch.hpp"
#include "Sector.hpp"

Sector::Sector(int x, int y) :
	lt_x{ x },
	lt_z{ y }
{
}

Sector::Sector(const Sector& other) :
	lt_x{ other.lt_x },
	lt_z{ other.lt_z }
{
	std::memcpy(&objects, &other.objects, other.objects.size());
}

Sector& Sector::operator=(const Sector& other)
{
	if (this != &other)
	{
		lt_x = other.lt_x;
		lt_z = other.lt_z;

		std::memcpy(&objects, &other.objects, other.objects.size());
	}

	return *this;
}

Sector::Sector(Sector&& other) noexcept :
	lt_x{ other.lt_x },
	lt_z{ other.lt_z }
{
	objects.swap(other.objects);

	lt_x = 0;
	lt_z = 0;
	other.objects.clear();
}

Sector& Sector::operator=(Sector&& other) noexcept
{
	if (this != &other)
	{
		lt_x = other.lt_x;
		lt_z = other.lt_z;
		objects.swap(other.objects);

		other.lt_x = 0;
		other.lt_z = 0;
		other.objects.clear();
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

c_set Sector::MakeViewList(Session* session, c_map* others)
{
	c_set new_list;

	if (!objects.empty())
	{
		for (auto& other : *others)
		{
			if (not session->IsMyID(other.first))
			{
				POS disX{ session->GetMyObject()->GetX() - other.second->GetMyObject()->GetX() };
				POS disZ{ session->GetMyObject()->GetZ() - other.second->GetMyObject()->GetZ() };

				if (IsInSight(disX, disZ))
				{
					new_list.insert(other.first);
				}
			}
		}
	}

	return new_list;
}

bool Sector::OutOfSectorXL(float x)
{
	if (x >= lt_x)
		return false;

	return true;
}

bool Sector::OutOfSectorZD(float z)
{
	if (z >= lt_z)
		return false;

	return true;
}

bool Sector::OutOfSectorXR(float x)
{
	if (x < lt_x + SECTOR_RANGE)
		return false;

	return true;
}

bool Sector::OutOfSectorZU(float z)
{
	if (z < lt_z + SECTOR_RANGE)
		return false;

	return true;
}
