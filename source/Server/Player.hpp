#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include "Object.hpp"

class Player : public Object
{
public:
	Player() = default;
	Player(POS x, POS y, POS z);
	~Player() = default;

	void Move(int direction) override;
	void Attack(int interaction);
	void Rotate(float cx, float cy);

	constexpr XMFLOAT3 GetLook() const noexcept { return look; }
	constexpr XMFLOAT3 GetRight() const noexcept { return right; }
	constexpr XMFLOAT3 GetUp() const noexcept { return up; }

	void SetLook(XMFLOAT3 new_look) { look = new_look; }
	void SetRight(XMFLOAT3 new_right) { right = new_right; }
	void SetUp(XMFLOAT3 new_up) { up = new_up; }
	void SetPitch(float new_pitch) { pitch = new_pitch; }
	void SetYaw(float new_yaw) { yaw = new_yaw; }

private:
	XMFLOAT3 look;			// Look Vector
	XMFLOAT3 right;			// Right Vector
	XMFLOAT3 up;			// Up Vector

	float pitch;
	float yaw;
};

#endif // !_PLAYER_HPP_
