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

	void Move(DIRECTION direction);

	short GetPosX() { return static_cast<short>(x); }
	short GetPosY() { return static_cast<short>(y); }
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
	int x, y, z;
};

#endif // !_PLAYER_HPP_
