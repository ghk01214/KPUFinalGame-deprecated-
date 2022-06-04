#include "pch.h"
#include "Zone.h"

Zone::Zone() :
#pragma region CONSTRUCTOR
	width{ VAR::WORLD_XR - VAR::WORLD_XL },
	height{ VAR::WORLD_ZF - VAR::WORLD_ZB }
{
	sector_num_x = static_cast<int>(width) / SECTOR_RANGE;
	sector_num_z = static_cast<int>(height) / SECTOR_RANGE;

	if (static_cast<int>(width) % SECTOR_RANGE != 0)
	{
		++sector_num_x;
	}

	if (static_cast<int>(height) % SECTOR_RANGE != 0)
	{
		++sector_num_z;
	}

	sector.reserve(sector_num_z);

	for (int z = 0, lb_z = VAR::WORLD_ZB; z < sector.capacity(); ++z, lb_z += SECTOR_RANGE)
	{
		std::vector<Sector> temp;
		temp.reserve(sector_num_x);

		for (int x = 0, lb_x = VAR::WORLD_XL; x < temp.capacity(); ++x, lb_x += SECTOR_RANGE)
		{
			temp.emplace_back(lb_x, lb_z);
		}

		sector.emplace_back(temp);
	}
}
#pragma endregion

Zone::~Zone()
{
	for (auto& iter : objects)
	{
		iter.second = nullptr;
	}

	zone_lock.lock();
	objects.clear();
	zone_lock.unlock();
}

void Zone::SetInSector(int id)
{
	auto obj_x{ objects[id]->GetX() };
	auto obj_z{ objects[id]->GetZ() };

	for (int z = 0; z < sector.size(); ++z)
	{
		for (int x = 0; x < sector[z].size(); ++x)
		{
			// 플레이어가 있는 sector의 index 추출
			if (sector[z][x].GetLBX() <= obj_x &&
				obj_x < sector[z][x].GetLBX() + SECTOR_RANGE)
			{
				if (sector[z][x].GetLBZ() <= obj_z &&
					obj_z < sector[z][x].GetLBZ() + SECTOR_RANGE)
				{
					objects[id]->SetSectorIndex(x, z);

					break;
				}
			}
		}
	}
}

int Zone::UpdateSector(Session* client)
{
	int moved{ 0 };

	int id{ client->GetId() };

	auto& x{ client->sector_index_x };
	auto& z{ client->sector_index_z };

	if (sector[z][x].OutOfSectorXR(client->GetX()))
	{
		if (x < sector_num_x - 1)
		{
			sector[z][x].LeaveSector(id);
			sector[z][++x].EnterSector(id);
			++moved;
		}
	}

	if (sector[z][x].OutOfSectorXL(client->GetX()))
	{
		if (x > 0)
		{
			sector[z][x].LeaveSector(id);
			sector[z][--x].EnterSector(id);
			++moved;
		}
	}

	if (sector[z][x].OutOfSectorZF(client->GetZ()))
	{
		if (z < sector_num_z - 1)
		{
			sector[z][x].LeaveSector(id);
			sector[++z][x].EnterSector(id);
			++moved;
		}
	}

	if (sector[z][x].OutOfSectorZB(client->GetZ()))
	{
		if (z > 0)
		{
			sector[z][x].LeaveSector(id);
			sector[--z][x].EnterSector(id);
			++moved;
		}
	}

	return moved;
}

void Zone::AddObject(Session* client)
{
	objects[client->GetId()] = client;

	SetInSector(client->GetId());
}

void Zone::RemoveObject(int id)
{
	zone_lock.lock();
	objects.unsafe_erase(id);
	zone_lock.unlock();
}

void Zone::MoveObject(int id, int direction)
{
	auto my_obj{ objects[id] };

	my_obj->Move(direction);
	my_obj->SendMove(my_obj);

	if (UpdateSector(my_obj))
	{
		auto& x{ my_obj->sector_index_x };
		auto& z{ my_obj->sector_index_z };

		my_obj->view_lock.lock();
		auto old_list{ my_obj->view_list };
		my_obj->view_lock.unlock();

		c_set new_list;
		sector[z][x].MakeNewViewList(&new_list, my_obj, &objects);

		// 새로운 view list에
		for (auto& opp_id : new_list)
		{
			auto opp_obj{ objects[opp_id] };

			if (opp_obj->GetState() == STATE::INGAME)
			{
				// 내 view list에 상대 id가 없으면
				if (not my_obj->IsInViewList(opp_id))
				{
					// view list에 추가 후 상대 정보 전송
					my_obj->AddToViewList(opp_id);
					my_obj->SendAdd(opp_obj);
				}

				// 상대 view list에 내 id가 없으면
				if (opp_obj->IsInViewList(id))
				{
					opp_obj->SendMove(my_obj);
				}
				// 상대 view list에 내 id가 있으면
				else
				{
					// view list에 추가 후 내 정보 전송
					opp_obj->AddToViewList(id);
					opp_obj->SendAdd(my_obj);
				}
			}
		}

		// 기존의 veiw list에
		for (auto& opp_id : old_list)
		{
			auto opp_obj{ objects[opp_id] };

			if (opp_obj->GetState() == STATE::INGAME)
			{
				// 새로운 view list에 opp_id의 객체가 없으면
				if (new_list.find(opp_id) == new_list.end())
				{
					// view list에서 제거 후 제거 신호 전송
					my_obj->DeleteFromViewList(opp_id);
					my_obj->SendDelete(opp_id);

					// 상대 view list에 내 id가 있으면
					if (opp_obj->IsInViewList(id))
					{
						// view list에서 내 id 제거 후 제거 신호 전송
						opp_obj->DeleteFromViewList(id);
						opp_obj->SendDelete(id);
					}
				}
			}
		}
	}
}
