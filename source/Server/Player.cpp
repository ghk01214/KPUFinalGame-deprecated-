#include "pch.h"
#include "Player.h"

Player::Player() :
	max_speed{ 5.0f },
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

	if (position.x < VAR::WORLD_XL)
	{
		x = VAR::WORLD_XL;
	}
	else if (position.x > VAR::WORLD_XR)
	{
		x = VAR::WORLD_XR;
	}
	else
	{
		x = position.x;
	}

	if (position.z < VAR::WORLD_ZB)
	{
		z = VAR::WORLD_ZB;
	}
	else if (position.z > VAR::WORLD_ZF)
	{
		z = VAR::WORLD_ZF;
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
