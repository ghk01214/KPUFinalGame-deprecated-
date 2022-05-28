#include "pch.hpp"
#include "protocol.hpp"
#include "Session.hpp"
#include "Zone.hpp"

Zone::Zone() :
	width{ VAR::WORLD_X },
	height{ VAR::WORLD_Z },
	sector_num_x{ static_cast<int>(width / SECTOR_RANGE) },
	sector_num_z{ static_cast<int>(height / SECTOR_RANGE) }
{
	if (static_cast<int>(width) % SECTOR_RANGE != 0)
	{
		++sector_num_x;
	}
	if (static_cast<int>(height) % SECTOR_RANGE != 0)
	{
		++sector_num_z;
	}

	sector.reserve(sector_num_z);

	for (int z = 0, lt_z = 0; z < sector.capacity(); ++z, lt_z += SECTOR_RANGE)
	{
		std::vector<Sector> temp;
		temp.reserve(sector_num_x);

		for (int x = 0, lt_x = 0; x < temp.capacity(); ++x, lt_x += SECTOR_RANGE)
		{
			temp.emplace_back(lt_x, lt_z);
		}

		sector.emplace_back(temp);
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
				new_x < sector[z][x].GetLTX() + SECTOR_RANGE)
			{
				if (new_z >= sector[z][x].GetLTZ() &&
					new_z < sector[z][x].GetLTZ() + SECTOR_RANGE)
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

	auto x{ session->GetSectorIndexX() };
	auto z{ session->GetSectorIndexZ() };

	// 플레이어가 기존 sector 밖으로 이동한 경우
	// +x축 방향
	if (sector[*z][*x].OutOfSectorXR(player->GetX()))
	{
		if (*x < sector_num_x - 1)
		{
			sector[*z][(*x)++].LeaveSector(id);
			sector[*z][*x].EnterSector(id);

#ifdef DEBUG_GAME
			std::cout << id << "; " << "sector : " << *x << ", " << *z << std::endl;
#endif
		}
	}
	// -x축 방향
	else if (sector[*z][*x].OutOfSectorXL(player->GetX()))
	{
		if (*x > 0)
		{
			sector[*z][(*x)--].LeaveSector(id);
			sector[*z][*x].EnterSector(id);

#ifdef DEBUG_GAME
			std::cout << id << "; " << "sector : " << *x << ", " << *z << std::endl;
#endif
		}
	}

	// +z축 방향
	if (sector[*z][*x].OutOfSectorZU(player->GetZ()))
	{
		if (*x < sector_num_z - 1)
		{
			sector[(*z)++][*x].LeaveSector(id);
			sector[*z][*x].EnterSector(id);

#ifdef DEBUG_GAME
			std::cout << id << "; " << "sector : " << *x << ", " << *z << std::endl;
#endif
		}
	}
	// -z축 방향
	else if (sector[*z][*x].OutOfSectorZD(player->GetZ()))
	{
		if (*x > 0)
		{
			sector[(*z)--][*x].LeaveSector(id);
			sector[*z][*x].EnterSector(id);

#ifdef DEBUG_GAME
			std::cout << id << "; " << "sector : " << *x << ", " << *z << std::endl;
#endif
		}
	}
}

void Zone::AddObject(int id, Session* session)
{
	objects[id] = session;

	SetInSector(id);
}

void Zone::MovePlayer(int id, int direction)
{
	auto obj{ objects[id] };

	obj->GetMyObject()->Move(direction);

	//std::cout << id << ": " << obj->GetX() << ", " << obj->GetZ() << std::endl;

	// 움직인 플레이어에게 우선 이동 패킷 전송
	objects[id]->SendMoveObjectPacket(id, obj->GetMyObject());

	// 이동 후 sector 업데이트
	UpdateSector(obj);

	// 해당 object가 위치한 sector의 인덱스 좌표
	auto& x{ *obj->GetSectorIndexX() };
	auto& z{ *obj->GetSectorIndexZ() };

	// TODO: view list를 이용한 플레이어 추가 패킷 전송 여부 결정
	auto old_list{ obj->view_list };
	// 해당 zone 내의 모든 object에 대해 새로운 view list 생성
	auto new_list{ sector[z][x].MakeViewList(obj, &objects) };

	// 새로운 view list의 모든 객체에 대하여
	for (auto& opponent_id : new_list)
	{
		// 상대 npc
		auto opp_obj{ objects[opponent_id] };

		if (opp_obj->GetState() == STATE::INGAME)
		{
			// 상대의 view_list
			auto opponent_list{ opp_obj->view_list };

			// 나의 view list에 상대 id가 없으면
			if (not opp_obj->IsInMyViewList(opponent_id))
			{
				// view list에 추가
				obj->AddToViewList(opponent_id);

				// 나에게 해당 플레이어 정보 전송
				obj->SendAddObjectPacket(opponent_id, opp_obj->GetMyObject());
			}

			// 상대 view list에 나의 id가 있으면
			if (opp_obj->IsInMyViewList(id))
			{
				// 상대 클라이언트에서 나를 이동
				objects[opponent_id]->SendMoveObjectPacket(id, obj->GetMyObject());
			}
			// 상대 view list에 나의 id가 없으면
			else
			{
				// 상대 view list에 나를 추가
				opp_obj->AddToViewList(id);
				// 상대 클라이언트에 나의 정보 전송
				opp_obj->SendAddObjectPacket(id, obj->GetMyObject());
			}
		}
	}

	// 기존의 view list에 있는 모든 객체에 대하여
	for (auto& obj_id : old_list)
	{
		auto opp_obj{ objects[obj_id] };

		if (objects[obj_id]->GetState() == STATE::INGAME)
		{
			// 새로운 view list에 없으면
			if (new_list.find(obj_id) == new_list.end())
			{
				// 나의 view list에서 제거
				obj->RemoveFromViewList(obj_id);

				// 나에게 상대 정보 제거 신호 전송
				obj->SendRemoveObjectPacket(obj_id);

				// 상대 view list에 나의 id가 있으면
				if (opp_obj->IsInMyViewList(id))
				{
					// 상대 view list에서 제거
					opp_obj->RemoveFromViewList(id);
					// 상대 클라이언트에 나의 정보 제거 신호 전송
					opp_obj->SendRemoveObjectPacket(id);
				}
			}
		}
	}
}
