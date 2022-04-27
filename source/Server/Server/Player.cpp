#include "pch.hpp"
#include "Player.hpp"

CPlayer::CPlayer()
{
	name[0] = 0;
}

CPlayer::CPlayer(short x, short y) : x(x), y(y)
{
	name[0] = 0;
}

CPlayer::CPlayer(char* new_name)
{
	strcpy_s(name, new_name);
}

CPlayer::CPlayer(short x, short y, char* new_name) : x(x), y(y)
{
	strcpy_s(name, new_name);
}

void CPlayer::Move(DIRECTION direction)
{
	switch (direction)
	{
	case DIRECTION::UP:
	{
		if (y > 0)
		{
			--y;
		}
	}
	break;
	case DIRECTION::DOWN:
	{
		if (y < VAR_SIZE::WORLD_HEIGHT - 1)
		{
			++y;
		}
	}
	break;
	case DIRECTION::LEFT:
	{
		if (x > 0)
		{
			--x;
		}
	}
	break;
	case DIRECTION::RIGHT:
	{
		if (x < VAR_SIZE::WORLD_WIDTH - 1)
		{
			++x;
		}
	}
	break;
	default:
		break;
	}
}
