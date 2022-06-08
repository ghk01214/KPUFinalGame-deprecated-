#include "pch.h"
#include "Player.h"

Player::Player() :
	max_speed{ 2.25f },
	look{ 0.0f, 0.0f, 1.0f },
	right{ 1.0f, 0.0f, 0.0f },
	up{ 0.0f, 1.0f, 0.0f },
	pitch{ 0.0f },
	yaw{ 0.0f }
{
}

Player::~Player()
{
}

void Player::Move(int direction)
{
	XMFLOAT3 shift{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 temp_look{ look.x, 0.0f, look.z };

	if (direction & KEY::FORWARD)
	{
		DXMATH::AddVector(&shift, &temp_look, -max_speed);
	}
	if (direction & KEY::BACKWARD)
	{
		DXMATH::AddVector(&shift, &temp_look, +max_speed);
	}
	if (direction & KEY::LEFT)
	{
		DXMATH::AddVector(&shift, &right, -max_speed);
	}
	if (direction & KEY::RIGHT)
	{
		DXMATH::AddVector(&shift, &right, max_speed);
	}

	XMFLOAT3 position{ x, y, z };
	DXMATH::AddVector(&position, &shift);

	if (position.x < VAR::WORLD_X_MIN)
	{
		x = VAR::WORLD_X_MIN;
	}
	else if (position.x > VAR::WORLD_X_MAX)
	{
		x = VAR::WORLD_X_MAX;
	}
	else
	{
		x = position.x;
	}

	if (position.z < VAR::WORLD_Z_MIN)
	{
		z = VAR::WORLD_Z_MIN;
	}
	else if (position.z > VAR::WORLD_Z_MAX)
	{
		z = VAR::WORLD_Z_MAX;
	}
	else
	{
		z = position.z;
	}

	y = position.y;

#ifdef DEBUG
	//std::cout << x << ", " << z << std::endl;
#endif
}

void Player::Rotate(float cx, float cy)
{
	if (cx != 0.0f)
	{
		pitch += cx;

		if (pitch > 89.0f)
		{
			cx -= (pitch - 89.0f);
			pitch = 89.0f;
		}
		else if (pitch < -89.0f)
		{
			cx -= (pitch + 89.0f);
			pitch = -89.0f;
		}
	}
	if (cy != 0.0f)
	{
		yaw += cy;

		if (yaw > 360.0f)
		{
			yaw -= 360.0f;
		}
		else if (yaw < 0.0f)
		{
			yaw += 360.0f;
		}
	}

	if (cy != 0.0f)
	{
		XMMATRIX rotate{ XMMatrixRotationY(XMConvertToRadians(cy)) };
		DXMATH::TransformNormal(&look, &rotate);
		DXMATH::TransformNormal(&up, &rotate);
		DXMATH::TransformNormal(&right, &rotate);
	}

	if (cx != 0.0f)
	{
		XMMATRIX rotate{ XMMatrixRotationAxis(XMLoadFloat3(&right), XMConvertToRadians(cx)) };
		DXMATH::TransformNormal(&look, &rotate);
		DXMATH::TransformNormal(&up, &rotate);
		DXMATH::TransformNormal(&right, &rotate);
	}

	DXMATH::Normalize(&look);
	DXMATH::CrossNormalize(&right, &up, &look);
	DXMATH::CrossNormalize(&up, &look, &right);
}

void Player::Reset()
{
	look = { 0.0f, 0.0f, 1.0f };
	right = { 1.0f, 0.0f, 0.0f };
	up = { 0.0f, 1.0f, 0.0f };

	pitch = 0.0f;
	yaw = 0.0f;

	max_speed = 2.25f;

	Object::Reset();
}
