#include "pch.h"
#include "Lua.h"

Lua* Lua::instance{ nullptr };

Lua::Lua(Zone* zone) :
	zone{ zone }
{
}

Lua::~Lua()
{
	if (instance)
	{
		delete instance;
		instance = nullptr;
	}
}

void Lua::MakeNewInstance(Zone* zone)
{
	instance = new Lua{ zone };
}

int Lua::ScriptAPIGetObjX(lua_State* L)
{
	int id{ LUA::LuaInt(L, -1) };
	lua_pop(L, 2);

	POS x{ instance->zone->GetSession(id)->GetX() };

	lua_pushnumber(L, x);

	// push한 인자의 개수 return
	return 1;
}

int Lua::ScriptAPIGetObjY(lua_State* L)
{
	int id{ LUA::LuaInt(L, -1) };
	lua_pop(L, 2);

	POS y{ instance->zone->GetSession(id)->GetY() };

	lua_pushnumber(L, y);

	// push한 인자의 개수 return
	return 1;
}

int Lua::ScriptAPIGetObjZ(lua_State* L)
{
	int id{ LUA::LuaInt(L, -1) };
	lua_pop(L, 2);

	POS z{ instance->zone->GetSession(id)->GetZ() };

	lua_pushnumber(L, z);

	// push한 인자의 개수 return
	return 1;
}
