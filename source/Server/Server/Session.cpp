#include "pch.hpp"
#include "Player.hpp"
#include "Session.hpp"

Session::Session() :
	id{ -1 },
	object{ new Object{} },
	sock{ INVALID_SOCKET },
	state{ SESSION_STATE::FREE },
	remain_size{ 0 },
	flag{ 0 }
{
}

Session::Session(Object* obj) :
	id{ -1 },
	object{ obj },
	sock{ INVALID_SOCKET },
	state{ SESSION_STATE::FREE },
	remain_size{ 0 },
	flag{ 0 }
{
}

Session::~Session()
{
	if (object)
	{
		delete object;
		object = nullptr;
	}

	closesocket(sock);
}

void Session::Reset()
{
	closesocket(sock);

	id = -1;
	//object->Reset();
	sock = INVALID_SOCKET;
	state = SESSION_STATE::FREE;
	remain_size = 0;
	flag = 0;
}

void Session::OnDestroy()
{
	closesocket(sock);
}

void Session::RecvData()
{
	flag = 0;
	ZeroMemory(&recv_ex.over, sizeof(recv_ex.over));

	recv_ex.wsa_buf.buf = recv_ex.data + remain_size;
	recv_ex.wsa_buf.len = VAR_SIZE::DATA - remain_size;

	WSARecv(sock, &recv_ex.wsa_buf, 1, 0, &flag, &recv_ex.over, nullptr);
}

void Session::SendData(void* packet)
{
	send_ex.Set(reinterpret_cast<char*>(packet));

	WSASend(sock, &send_ex.wsa_buf, 1, 0, 0, &send_ex.over, nullptr);
}

void Session::SendLoginPakcet()
{
	sc_login_packet.size = sizeof(SC::PACKET::LOGIN);
	sc_login_packet.type = SC::LOGIN;
	sc_login_packet.id = id;
	sc_login_packet.x = object->GetX();
	sc_login_packet.y = object->GetY();
	sc_login_packet.z = object->GetZ();

	SendData(&sc_login_packet);
}

void Session::SendMoveObjectPacket(int id, Object* obj)
{
	sc_move_object_packet.size = sizeof(SC::PACKET::MOVE_OBJECT);
	sc_move_object_packet.type = SC::MOVE_OBJECT;
	sc_move_object_packet.id = id;
	sc_move_object_packet.x = obj->GetX();
	sc_move_object_packet.y = obj->GetY();
	sc_move_object_packet.z = obj->GetZ();

	SendData(&sc_move_object_packet);
}

void Session::SendAddObjectPacket(int id, Object* obj)
{
	sc_add_object_packet.size = sizeof(SC::PACKET::ADD_OBJECT);
	sc_add_object_packet.type = SC::ADD_OBJECT;
	sc_add_object_packet.id = id;
	obj->GetName().copy(sc_add_object_packet.name, sizeof(sc_add_object_packet.name));
	sc_add_object_packet.x = obj->GetX();
	sc_add_object_packet.y = obj->GetY();
	sc_add_object_packet.z = obj->GetZ();
	
	if (id < MAX_USER)
	{
		Player* player{ dynamic_cast<Player*>(obj) };

		sc_add_object_packet.look_x = player->GetLook().x;
		sc_add_object_packet.look_y = player->GetLook().y;
		sc_add_object_packet.look_z = player->GetLook().z;

		sc_add_object_packet.right_x = player->GetRight().x;
		sc_add_object_packet.right_y = player->GetRight().y;
		sc_add_object_packet.right_z = player->GetRight().z;

		sc_add_object_packet.up_x = player->GetUp().x;
		sc_add_object_packet.up_y = player->GetUp().y;
		sc_add_object_packet.up_z = player->GetUp().z;
	}
	else if (MAX_USER <= id and id < NPC_NUM)
	{
		NPC* npc{ dynamic_cast<NPC*>(obj) };

		sc_add_object_packet.look_x = npc->GetLook().x;
		sc_add_object_packet.look_y = npc->GetLook().y;
		sc_add_object_packet.look_z = npc->GetLook().z;

		sc_add_object_packet.right_x = npc->GetRight().x;
		sc_add_object_packet.right_y = npc->GetRight().y;
		sc_add_object_packet.right_z = npc->GetRight().z;

		sc_add_object_packet.up_x = npc->GetUp().x;
		sc_add_object_packet.up_y = npc->GetUp().y;
		sc_add_object_packet.up_z = npc->GetUp().z;
	}

	SendData(&sc_add_object_packet);
}

void Session::SendRemoveObjectPacket(int id)
{
	sc_remove_object_packet.size = sizeof(SC::PACKET::REMOVE_OBJECT);
	sc_remove_object_packet.type = SC::REMOVE_OBJECT;
	sc_remove_object_packet.id = id;

	SendData(&sc_remove_object_packet);
}

void Session::AddToViewList(int id)
{
	view_list.insert(id);
}

void Session::RemoveFromViewList(int id)
{
	std::lock_guard<std::shared_mutex> lock{ view_lock };
	view_list.unsafe_erase(id);
}
