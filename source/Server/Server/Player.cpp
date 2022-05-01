#include "pch.hpp"
#include "Player.hpp"

CPlayer::CPlayer()
{
	name[0] = 0;
}

CPlayer::CPlayer(short x, short y, short z) : x(x), y(y), z(z)
{
	name[0] = 0;
}

CPlayer::CPlayer(char* new_name)
{
	strcpy_s(name, new_name);
}

CPlayer::CPlayer(short x, short y, short z, char* new_name) : x(x), y(y), z(z)
{
	strcpy_s(name, new_name);
}

void CPlayer::Move(DIRECTION direction)
{
	switch (direction)
	{
	case DIRECTION::FORWARD:
	{
		++z;
	}
	break;
	case DIRECTION::BACKWARD:
	{
		--z;
	}
	break;
	case DIRECTION::LEFT:
	{
		--x;
	}
	break;
	case DIRECTION::RIGHT:
	{
		++x;
	}
	break;
	case DIRECTION::UP:
	{
		--y;
	}
	break;
	case DIRECTION::DOWN:
	{
		++y;
	}
	break;
	default:
		break;
	}

	std::cout << x << ", " << y << ", " << z << std::endl;
}
