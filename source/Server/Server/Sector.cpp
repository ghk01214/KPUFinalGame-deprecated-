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

concurrency::concurrent_unordered_set<int> Sector::MakeViewList(Session* session, concurrency::concurrent_unordered_map<int, Session*>* others)
{
	concurrency::concurrent_unordered_set<int> new_list;

	if (!objects.empty())
	{
		sector_lock.lock_shared();

		for (auto& other : *others)
		{
			if (other.second->GetID() != -1)
			{
				if (session->GetID() == other.second->GetID())
				{
					continue;
				}

				if (IsInSight(session, other.second))
				{
					new_list.insert(other.second->GetID());
				}
			}
		}

		sector_lock.unlock_shared();
	}

	return new_list;
}
