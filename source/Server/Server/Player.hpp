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
	void Rotate(float cy) override;
	void Attack(int interaction);

private:
};

#endif // !_PLAYER_HPP_
