#include "pch.hpp"
#include "protocol.hpp"
#include "Session.hpp"
#include "Zone.hpp"

Zone::Zone() :
	zone_width{ VAR_SIZE::WORLD_X },
	zone_height{ VAR_SIZE::WORLD_Z },
	sector_num_x{ static_cast<int>(VAR_SIZE::WORLD_X) / SECTOR_RANGE + 1 },
	sector_num_z{ static_cast<int>(VAR_SIZE::WORLD_Z) / SECTOR_RANGE + 1 }
{
	for (int y = 0, lt_z = 0; y < sector.size(); ++y, lt_z += SECTOR_RANGE)
	{
		for (int x = 0, lt_x = 0; x < sector[y].size(); ++x, lt_x += SECTOR_RANGE)
		{
			sector[y][x] = std::move(Sector{ lt_x, lt_z });
		}
	}
}

Zone::~Zone()
{
}

void Zone::SetInSector(int id)
{
	auto new_x{ objects[id]->GetMyObject()->GetX() };
	auto new_z{ objects[id]->GetMyObject()->GetZ() };

	for (int z = 0; z < sector.size(); ++z)
	{
		for (int x = 0; x < sector[z].size(); ++x)
		{
			// 플레이어가 해당 sector 안에 있으면 sector의 인덱스 추출
			if (new_x >= sector[z][x].GetLTX() &&
				new_x <= sector[z][x].GetLTX() + SECTOR_RANGE)
			{
				if (new_z >= sector[z][x].GetLTZ() &&
					new_z <= sector[z][x].GetLTZ() + SECTOR_RANGE)
				{
					objects[id]->SetSectorIndex(x, z);

					break;
				}
			}
		}
	}
}

void Zone::UpdateSector(Session* session)
{
	auto player{ session->GetMyObject() };
	auto id{ session->GetID() };

	int* index_x{ session->GetSectorIndexX() };
	int* index_z{ session->GetSectorIndexZ() };

	// 플레이어가 기존 sector 밖으로 이동한 경우
	// +x축 방향
	if (player->GetX() > sector[*index_z][*index_x].GetLTX() + SECTOR_RANGE)
	{
		sector[*index_z][*index_x].LeaveSector(id);

		if (*index_x < sector_num_x)
		{
			sector[*index_z][++(*index_x)].EnterSector(id);

#ifdef DEBUG_GAME
			std::cout << "sector : " << *index_x << ", " << *index_z << std::endl;
#endif
		}
	}
	// -x축 방향
	else if (player->GetX() < sector[*index_z][*index_x].GetLTX())
	{
		sector[*index_z][*index_x].LeaveSector(id);

		if (*index_x > 0)
		{
			sector[*index_z][--(*index_x)].EnterSector(id);

#ifdef DEBUG_GAME
			std::cout << "sector : " << *index_x << ", " << *index_z << std::endl;
#endif
		}
	}

	// +z축 방향
	if (player->GetZ() > sector[*index_z][*index_x].GetLTZ() + SECTOR_RANGE)
	{
		sector[*index_z][*index_x].LeaveSector(id);

		if (*index_z < sector_num_z)
		{
			sector[++(*index_z)][*index_x].EnterSector(id);

#ifdef DEBUG_GAME
			std::cout << "sector : " << *index_x << ", " << *index_z << std::endl;
#endif
		}
	}
	// -z축 방향
	else if (player->GetZ() < sector[*index_z][*index_x].GetLTZ())
	{
		sector[*index_z][*index_x].LeaveSector(id);

		if (*index_z > 0)
		{
			sector[--(*index_z)][*index_x].EnterSector(id);

#ifdef DEBUG_GAME
			std::cout << "sector : " << *index_x << ", " << *index_z << std::endl;
#endif
		}
	}
}

void Zone::AddObject(int id, Session* session)
{
	objects[id] = session;
}

void Zone::MoveObject(int id, int direction)
{
	auto obj{ objects[id]->GetMyObject() };

	// object가 월드 안에 있을 경우에만 이동
	if (obj->IsInWorld())
	{
		obj->Move(direction);

		//std::cout << id << ": " << obj->GetX() << ", " << obj->GetZ() << std::endl;

		// 움직인 플레이어에게 우선 이동 패킷 전송
		objects[id]->SendMoveObjectPacket(id, obj);

		// 이동 후 sector 업데이트
		UpdateSector(objects[id]);

		// 해당 object가 위치한 sector의 인덱스 좌표
		auto& index_x{ *objects[id]->GetSectorIndexX() };
		auto& index_y{ *objects[id]->GetSectorIndexZ() };

		// TODO: view list를 이용한 플레이어 추가 패킷 전송 여부 결정
		auto old_list{ objects[id]->GetViewList() };
		// 해당 zone 내의 모든 object에 대해 새로운 view list 생성
		auto new_list{ sector[index_y][index_x].MakeViewList(objects[id], &objects) };

		// 새로운 view list의 모든 객체에 대하여
		for (auto& opponent_id : new_list)
		{
			objects[opponent_id]->state_lock.lock();

			if (objects[opponent_id]->GetState() == SESSION_STATE::INGAME)
			{
				// 상대의 view_list
				auto opponent_list{ objects[opponent_id]->GetViewList() };

				// 나의 view list에 상대 id가 없으면
				if (old_list->find(opponent_id) == old_list->end())
				{
					// view list에 추가
					objects[id]->AddToViewList(opponent_id);

					// 나에게 해당 플레이어 정보 전송
					objects[id]->SendAddObjectPacket(opponent_id, objects[opponent_id]->GetMyObject());

					// 상대 view list에 나의 id가 있으면
					if (opponent_list->find(id) != opponent_list->end())
					{
						// 상대 클라이언트에서 나를 이동
						objects[opponent_id]->SendMoveObjectPacket(id, obj);
					}
					// 상대 view list에 나의 id가 없으면
					else
					{
						// 상대 view list에 나를 추가
						opponent_list->insert(id);

						// 상대 클라이언트에 나의 정보 전송
						objects[opponent_id]->SendAddObjectPacket(id, obj);
					}
				}
				// 나의 view list에 상대 id가 있으면
				else
				{
					// 상대 view list에 있으면
					if (opponent_list->find(id) != opponent_list->end())
					{
						objects[opponent_id]->SendMoveObjectPacket(id, obj);
					}
					// 상대 view list에 없으면
					else
					{
						objects[opponent_id]->AddToViewList(id);

						objects[opponent_id]->SendAddObjectPacket(id, obj);
					}
				}
			}

			objects[opponent_id]->state_lock.unlock();
		}

		// 기존의 view list에 있는 모든 객체에 대하여
		for (auto& obj_id : *old_list)
		{
			objects[obj_id]->state_lock.lock();

			if (objects[obj_id]->GetState() == SESSION_STATE::INGAME)
			{
				// 새로운 view list에 없으면
				if (new_list.find(obj_id) == new_list.end())
				{
					// 나의 view list에서 제거
					objects[id]->RemoveFromViewList(obj_id);

					// 나에게 상대 정보 제거 신호 전송
					objects[id]->SendRemoveObjectPacket(obj_id);

					// 상대 view list에 나의 id가 있으면
					if (objects[obj_id]->GetViewList()->find(id) != objects[obj_id]->GetViewList()->end())
					{
						// 상대 view list에서 제거
						objects[obj_id]->RemoveFromViewList(id);

						// 상대 클라이언트에 나의 정보 제거 신호 전송
						objects[obj_id]->SendRemoveObjectPacket(id);
					}
				}
			}

			objects[obj_id]->state_lock.unlock();
		}
	}
}
