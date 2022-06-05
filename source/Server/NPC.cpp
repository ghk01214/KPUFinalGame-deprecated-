#include "pch.h"
#include "NPC.h"

NPC::NPC()
{
}

NPC::NPC(POS x, POS y, POS z) :
	Object{ x, y, z },
	look{ 0.0f, 0.0f, 1.0f },
	right{ 1.0f, 0.0f, 0.0f },
	up{ 0.0f, 1.0f, 0.0f }
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

void NPC::Move()
{
	// A* 알고리즘
}
