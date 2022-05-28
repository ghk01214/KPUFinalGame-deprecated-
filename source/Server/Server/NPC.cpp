#include "pch.hpp"
#include "NPC.hpp"

NPC::NPC(POS x, POS y, POS z) :
	Object{ x, y, z },
	look{ XMFLOAT3{ 0.f, 0.f, 1.f } },
	right{ XMFLOAT3{ 1.f, 0.f, 0.f } },
	up{ XMFLOAT3{ 0.f, 1.f, 0.f } },
	L{ nullptr }
{
	L = luaL_newstate();

	luaL_openlibs(L);
	luaL_loadfile(L, "NPC.lua");
	lua_pcall(L, 0, 0, 0);

	// TODO: Lua 파일 내부에서 사용할 함수 등록
	// lua_register(L, Lua 내부 함수명, C++ 파일 내 함수명);
}

NPC::~NPC()
{
	lua_close(L);

	if (L)
	{
		delete L;
		L = nullptr;
	}
}

void NPC::InitializeScript(int id)
{
	lua_getglobal(L, "SetObjID");
	lua_pushnumber(L, id);
	lua_pcall(L, 1, 0, 0);
}

void NPC::Reset()
{
	Object::Reset();

	look.x = 0.f;
	look.y = 0.f;
	look.z = 0.f;

	right.x = 0.f;
	right.y = 0.f;
	right.z = 0.f;

	up.x = 0.f;
	up.y = 0.f;
	up.z = 0.f;
}
