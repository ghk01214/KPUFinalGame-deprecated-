#ifndef _NPC_HPP_
#define _NPC_HPP_

#include "Object.hpp"

class NPC : public Object
{
public:
	NPC() = default;
	NPC(POS x, POS y, POS z);
	~NPC();

	void InitializeScript(int id);
	void Reset() override;

	// TODO: Lua 내부에서 호출할 메서드

	virtual void Rotate(float cx, float cy) {}

	void SetLook(XMFLOAT3 new_look) { look = new_look; }
	void SetRight(XMFLOAT3 new_right) { right = new_right; }
	void SetUp(XMFLOAT3 new_up) { up = new_up; }

	constexpr XMFLOAT3 GetLook() const noexcept { return look; }
	constexpr XMFLOAT3 GetRight() const noexcept { return right; }
	constexpr XMFLOAT3 GetUp() const noexcept { return up; }

protected:
	XMFLOAT3 look;			// Look Vector
	XMFLOAT3 right;			// Right Vector
	XMFLOAT3 up;			// Up Vector
protected:
	lua_State* L;
};

#endif // !_NPC_HPP_
