#include "pch.hpp"
#include "Player.hpp"

CPlayer::CPlayer() : x(0), y(0), z(0)
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

void CPlayer::Move(int direction)
{
	int input_count{ 0 };

	// 플레이어 이동 속도 제어와 관련된 부분, 추후 사용 예정
	if (direction & DIRECTION::FORWARD)
		++input_count;
	if (direction & DIRECTION::BACKWARD)
		--input_count;
	if (direction & DIRECTION::RIGHT)
		++input_count;
	if (direction & DIRECTION::LEFT)
		--input_count;

	//===============================

	if (direction & DIRECTION::FORWARD)
	{
		z += 5.0f;
	}
	if (direction & DIRECTION::BACKWARD)
	{
		z -= 5.0f;
	}
	if (direction & DIRECTION::RIGHT)
	{
		x += 5.0f;
	}
	if (direction & DIRECTION::LEFT)
	{
		x -= 5.0f;
	}

	// 디버깅 용 플레이어 좌표 출력 문구
	//std::cout << x << ", " << y << ", " << z << std::endl;
void CPlayer::Attack(int interaction)
{
}
