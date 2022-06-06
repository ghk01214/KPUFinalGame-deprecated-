#pragma once

#include "Object.h"

class Player : public Object
{
public:
	Player();
	~Player();

	void Move(int direction) override;
	void Rotate(float cx, float cy) override;

	constexpr XMFLOAT3 GetLook() const noexcept { return look; }
	constexpr XMFLOAT3 GetRight() const noexcept { return right; }
	constexpr XMFLOAT3 GetUp() const noexcept { return up; }

	void SetLook(XMFLOAT3 new_look) { look = new_look; }
	void SetRight(XMFLOAT3 new_right) { right = new_right; }
	void SetUp(XMFLOAT3 new_up) { up = new_up; }
	void SetPitch(float new_pitch) { pitch = new_pitch; }
	void SetYaw(float new_yaw) { yaw = new_yaw; }

private:
	XMFLOAT3 look;
	XMFLOAT3 right;
	XMFLOAT3 up;

	float pitch;
	float yaw;

	float max_speed;
};

