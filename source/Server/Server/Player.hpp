#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include "NPC.hpp"

class Player : public NPC
{
public:
	Player() = default;
	Player(POS x, POS y, POS z);
	~Player() = default;

	void Move(int direction) override;
	void Rotate(float cx, float cy) override;
	void Attack(int interaction);

	void SetPitch(float new_pitch) { pitch = new_pitch; }
	void SetYaw(float new_yaw) { yaw = new_yaw; }

private:
	float pitch;
	float yaw;
};

#endif // !_PLAYER_HPP_
