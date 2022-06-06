#pragma once

#include "Object.h"

class NPC : public Object
{
public:
	NPC();
	NPC(POS x, POS y, POS z);
	~NPC();

	void InitializeScript(int id);

	void Move();

	// TODO: Lua 내부에서 호출할 메서드

	constexpr XMFLOAT3 GetLook() const noexcept { return look; }
	constexpr XMFLOAT3 GetRight() const noexcept { return right; }
	constexpr XMFLOAT3 GetUp() const noexcept { return up; }

protected:
	XMFLOAT3 look;
	XMFLOAT3 right;
	XMFLOAT3 up;

	lua_State* L;
};

