#ifndef _LUA_SCRIPT_HPP_
#define _LUA_SCRIPT_HPP_

#include "Zone.hpp"

class LuaScript
{
public:
	LuaScript() = default;
	LuaScript(Zone* zone);
	~LuaScript();

	static void MakeNewInstance(Zone* zone);

	// Lua 내부에 등록할 메서드들은 전부 static int 형으로 선언
	// 인자로는 lua 가상 머신의 포인터만 받는다
	static int ScriptAPIGetObjX(lua_State* L);
	static int ScriptAPIGetObjY(lua_State* L);
	static int ScriptAPIGetObjZ(lua_State* L);

private:
	// static으로 선언된 메서드에서 멤버 변수를 불러오기 위해서는
	// static으로 선언된 this
	static LuaScript* instance;
private:
	Zone* zone;
};

#endif // !_LUA_SCRIPT_HPP_
