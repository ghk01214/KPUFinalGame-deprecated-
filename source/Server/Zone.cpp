#include "pch.h"
#include "NPC.h"
#include "Zone.h"

Zone::Zone() :
#pragma region CONSTRUCTOR
	width{ VAR::WORLD_X_MAX - VAR::WORLD_X_MIN },
	height{ VAR::WORLD_Z_MAX - VAR::WORLD_Z_MIN }
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

	for (int z = 0, lt_z = VAR::WORLD_Z_MIN; z < sector_num_z; ++z, lt_z += SECTOR_RANGE)
	{
		std::vector<Sector> temp;
		temp.reserve(sector_num_x);

		for (int x = 0, lt_x = VAR::WORLD_X_MIN; x < sector_num_x; ++x, lt_x += SECTOR_RANGE)
		{
			temp.emplace_back(lt_x, lt_z);
		}

		sector.emplace_back(temp);
	}
}
#pragma endregion

Zone::~Zone()
{
	zone_lock.lock();
	players.clear();
	npcs.clear();
	zone_lock.unlock();
}

void Zone::SetInSector(ID id, c_map* cont)
{
	auto obj_x{ (*cont)[id]->GetX() };
	auto obj_z{ (*cont)[id]->GetZ() };

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
					(*cont)[id]->sector_index_x = x;
					(*cont)[id]->sector_index_z = z;
					sector[z][x].EnterSector(id);
					std::cout << "스폰 위치 = [" << id << "] : sector(" << x << ", " << z << ")\n";
					break;
				}
			}
		}
	}
}

void Zone::AddPlayer(Session* client)
{
	players[client->GetId()] = client;

	SetInSector(client->GetId(), &players);
}

void Zone::AddNPC(Session* npc)
{
	npcs[npc->GetId()] = npc;

	SetInSector(npc->GetId(), &npcs);
}

void Zone::RemovePlayer(ID id)
{
	zone_lock.lock();
	players.unsafe_erase(id);
	zone_lock.unlock();
}

void Zone::UpdateSector(Session* client)
{
	int id{ client->GetId() };

	auto& x{ client->sector_index_x };
	auto& z{ client->sector_index_z };

	if (sector[z][x].OutOfSectorXMax(client->GetX()))
	{
		if (x < sector_num_x - 1)
		{
			sector[z][x].LeaveSector(id);
			sector[z][++x].EnterSector(id);

			std::cout << "[" << id << "] : sector(" << x << ", " << z << ")\n";
		}
	}
	else if (sector[z][x].OutOfSectorXMin(client->GetX()))
	{
		if (x > 0)
		{
			sector[z][x].LeaveSector(id);
			sector[z][--x].EnterSector(id);

			std::cout << "[" << id << "] : sector(" << x << ", " << z << ")\n";
		}
	}

	if (sector[z][x].OutOfSectorZMax(client->GetZ()))
	{
		if (z < sector_num_z - 1)
		{
			sector[z][x].LeaveSector(id);
			sector[++z][x].EnterSector(id);

			std::cout << "[" << id << "] : sector(" << x << ", " << z << ")\n";
		}
	}
	else if (sector[z][x].OutOfSectorZMin(client->GetZ()))
	{
		if (z > 0)
		{
			sector[z][x].LeaveSector(id);
			sector[--z][x].EnterSector(id);

			std::cout << "[" << id << "] : sector(" << x << ", " << z << ")\n";
		}
	}
}

void Zone::MovePlayer(ID id, int direction)
{
	auto my_obj{ players[id] };

	my_obj->Move(direction);
	my_obj->SendMove(my_obj);

	//std::cout << "[" << id << "] : " << my_obj->GetX() << ", " << my_obj->GetZ() << "\n";

	UpdateSector(my_obj);

	auto x{ my_obj->sector_index_x };
	auto z{ my_obj->sector_index_z };

	my_obj->view_lock.lock();
	auto old_list{ my_obj->view_list };
	my_obj->view_lock.unlock();

	c_set new_list;
	int x_start{ x - 1 }, x_end{ x + 2 };
	int z_start{ z - 1 }, z_end{ z + 2 };

	if (x == 0)
	{
		x_start = 0;
		x_end = x_start + 2;
	}
	else if (x_start == sector_num_x - 2)
	{
		x_end = sector_num_x;
		x_start = x_end - 2;
	}

	if (z == 0)
	{
		z_start = 0;
		z_end = z_start + 2;
	}
	else if (z_start == sector_num_z - 2)
	{
		z_end = sector_num_z;
		z_start = z_end - 2;
	}

	// 플레이어가 있는 5번 sector의 인접한 8개 sector를 검사해서 view list 생성
	//	 	 ┃	 	 ┃	 
	//	 1 	 ┃	 2 	 ┃	 3
	//	  	 ┃	  	 ┃	 
	// ━━━━━━╋━━━━━━━╋━━━━━━━
	//	 	 ┃	 	 ┃	 
	//	 4 	 ┃	 5 	 ┃	 6
	//	  	 ┃	  	 ┃	 
	// ━━━━━━╋━━━━━━━╋━━━━━━━
	//	 	 ┃	 	 ┃	 
	//	 7 	 ┃	 8 	 ┃	 9
	//	  	 ┃	  	 ┃	 
	for (int sec_z = z_start; sec_z < z_end; ++sec_z)
	{
		for (int sec_x = x_start; sec_x < x_end; ++sec_x)
		{
			sector[sec_z][sec_x].MakeNewPlayerViewList(&new_list, my_obj, &players, &npcs);
		}
	}

	// 새로운 view list에
	for (auto& opp_id : new_list)
	{
		auto opp_obj{ players[opp_id] };

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
		auto opp_obj{ players[opp_id] };

		if (opp_obj->GetState() == STATE::INGAME)
		{
			// 새로운 view list에 opp_id의 객체가 없으면
			if (new_list.find(opp_id) == new_list.end())
			{
				// view list에서 제거 후 제거 신호 전송
				my_obj->RemoveFromViewList(opp_id);
				my_obj->SendRemove(opp_id);

				// 상대 view list에 내 id가 있으면
				if (opp_obj->IsInViewList(id))
				{
					// view list에서 내 id 제거 후 제거 신호 전송
					opp_obj->RemoveFromViewList(id);
					opp_obj->SendRemove(id);
				}
			}
		}
	}
}

void Zone::MoveNPC()
{
	for (auto& [npc_id, npc] : npcs)
	{
		npc->Move();

		UpdateSector(npc);

		for (auto& [player_id, player] : players)
		{
			if (player->GetState() == STATE::INGAME)
			{
				player->SendMove(npc);
			}
		}
	}
}

void Zone::RotatePlayer(ID id, float cx, float cy)
{
	auto my_obj{ players[id] };

	my_obj->Rotate(cx, cy);

	for (auto& opp_id : my_obj->view_list)
	{
		auto opp_obj{ players[opp_id] };

		if (opp_obj->GetState() == STATE::INGAME)
		{
			opp_obj->SendRotate(my_obj);
		}
	}
}
