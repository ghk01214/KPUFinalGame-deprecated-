#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include "protocol.hpp"


class CPlayer
{
public:
	CPlayer();
	CPlayer(short x, short y, short z);
	CPlayer(char* new_name);
	CPlayer(short x, short y, short z, char* new_name);

	void Move(int direction, float look_x, float look_z, float right_x, float right_z);
	void Attack(int interaction);

	short GetPosX() { return static_cast<short>(x); }
	short GetPosY() { return static_cast<short>(y); }
	short GetPosZ() { return static_cast<short>(z); }
	char* GetName() { return name; }

	void SetPosX(int X) { x = X; }
	void SetPosY(int Y) { y = Y; }
	void SetPosZ(int Z) { z = Z; }
	void SetName(char* NAME) { 
		if (NAME == 0)
		{
			name[0] = 0;
			return;
		}
		strcpy_s(name, NAME); }

private:
	char name[VAR_SIZE::NAME];
	float x, y, z;

	float max_speed;
};

#endif // !_PLAYER_HPP_
