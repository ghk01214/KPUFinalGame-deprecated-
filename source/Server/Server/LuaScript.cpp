#include "pch.hpp"
#include "LuaScript.hpp"

LuaScript* LuaScript::instance = nullptr;

LuaScript::LuaScript(Zone* zone) :
	zone{ zone }
{
}

LuaScript::~LuaScript()
{
	delete instance;
}

void LuaScript::MakeNewInstance(Zone* zone)
{
	instance = new LuaScript{ zone };
}

int LuaScript::ScriptAPIGetObjX(lua_State* L)
{
	int id{ LuaInt(L, -1) };
	lua_pop(L, 2);

	POS x{ instance->zone->GetSession(id)->GetMyObject()->GetX() };

	lua_pushnumber(L, x);

	// push한 인자의 개수 return
	return 1;
}

int LuaScript::ScriptAPIGetObjY(lua_State* L)
{
	int id{ LuaInt(L, -1) };
	lua_pop(L, 2);

	POS y{ instance->zone->GetSession(id)->GetMyObject()->GetX() };

	lua_pushnumber(L, y);

	return 1;
}

int LuaScript::ScriptAPIGetObjZ(lua_State* L)
{
	int id{ LuaInt(L, -1) };
	lua_pop(L, 2);

	POS z{ instance->zone->GetSession(id)->GetMyObject()->GetX() };

	lua_pushnumber(L, z);

	return 1;
}
