#ifndef _NPC_HPP_
#define _NPC_HPP_

#include "Object.hpp"

class NPC : public Object
{
public:
	NPC() = default;
	NPC(POS x, POS y, POS z);
	~NPC() = default;

	virtual void Rotate(float cy) {}

	void SetLook(XMFLOAT3 new_look) { look = new_look; }
	void SetRight(XMFLOAT3 new_right) { right = new_right; }
	void SetUp(XMFLOAT3 new_up) { up = new_up; }

	XMFLOAT3 GetLook() { return look; }
	XMFLOAT3 GetRight() { return right; }
	XMFLOAT3 GetUp() { return up; }

protected:
	XMFLOAT3 look;			// Look Vector
	XMFLOAT3 right;		// Right Vector
	XMFLOAT3 up;			// Up Vector
};

#endif // !_NPC_HPP_
