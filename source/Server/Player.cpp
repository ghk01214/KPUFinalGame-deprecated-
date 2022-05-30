#include "pch.hpp"
#include "protocol.hpp"
#include "Player.hpp"

Player::Player(POS x, POS y, POS z) :
	pitch{ 0.0f },
	yaw{ 0.0f }
{
}

void Player::Move(int direction)
{
	XMFLOAT3 shift{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 temp_look{ look.x, 0.0f, look.z };

	if (direction & KEYINPUT::FORWARD)
	{
		DXMATH::AddVector(&shift, &temp_look, -max_speed);
	}
	if (direction & KEYINPUT::BACKWARD)
	{
		DXMATH::AddVector(&shift, &temp_look, +max_speed);
	}
	if (direction & KEYINPUT::LEFT)
	{
		DXMATH::AddVector(&shift, &right, -max_speed);
	}
	if (direction & KEYINPUT::RIGHT)
	{
		DXMATH::AddVector(&shift, &right, max_speed);
	}

	XMFLOAT3 position{ x, y, z };
	DXMATH::AddVector(&position, &shift);

	if (position.x < 0)
	{
		x = 0.0f;
	}
	else if (position.x > VAR::WORLD_X)
	{
		x = VAR::WORLD_X;
	}
	else
	{
		x = position.x;
	}

	if (position.z < 0)
	{
		z = 0.0f;
	}
	else if (position.z > VAR::WORLD_Z)
	{
		z = VAR::WORLD_Z;
	}
	else
	{
		z = position.z;
	}
	
	y = position.y;
	

#if _DEBUG
#ifdef DEBUG_GAME
	std::cout << x << ", " << z << std::endl;
#endif
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

void Player::Attack(int interaction)
{
}
