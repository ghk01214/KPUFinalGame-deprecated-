#include "pch.hpp"
#include "Player.hpp"

CPlayer::CPlayer() : x(0), y(0), z(0), max_speed(5.0f)
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

void CPlayer::Reset()
{
	x = y = z = 0;
	max_speed = 5.0f;
	name[0] = '\0';
	name[0] = 0;
}

void CPlayer::Move(int direction, float look_x, float look_z, float right_x, float right_z)
{
	int input_count{ 0 };

	// 플레이어 이동 속도 제어와 관련된 부분, 추후 사용 예정
	if (direction & KEYINPUT::FORWARD)
		++input_count;
	if (direction & KEYINPUT::BACKWARD)
		--input_count;
	if (direction & KEYINPUT::RIGHT)
		++input_count;
	if (direction & KEYINPUT::LEFT)
		--input_count;

	//===============================

	float temp_x{ 0.0f }, temp_z{ 0.0f };

	if (direction & KEYINPUT::FORWARD)
	{
		temp_x += look_x * max_speed;
		temp_z += look_z * max_speed;
	}
	if (direction & KEYINPUT::BACKWARD)
	{
		temp_x += look_x * max_speed * (-1);
		temp_z += look_z * max_speed * (-1);
	}
	if (direction & KEYINPUT::RIGHT)
	{
		temp_x += right_x * max_speed * (-1);
		temp_z += right_z * max_speed * (-1);
	}
	if (direction & KEYINPUT::LEFT)
	{
		temp_x += right_x * max_speed;
		temp_z += right_z * max_speed;
	}

	//x += temp_x;
	//z += temp_z;

	x = temp_x;
	z = temp_z;

	// 디버깅 용 플레이어 좌표 출력 문구
	//std::cout << x << ", " << z << std::endl;
}

void CPlayer::Attack(int interaction)
{
}
