#include "pch.hpp"
#include "protocol.hpp"
#include "Player.hpp"

Player::Player(POS x, POS y, POS z) : NPC{ x, y, z }
{
}

void Player::Move(int direction)
{
	XMFLOAT3 shift{ 0.0f, 0.0f, 0.0f };

	if (direction & KEYINPUT::FORWARD)
	{
		XMStoreFloat3(&shift, XMLoadFloat3(&shift) + (XMLoadFloat3(&look) * max_speed));
	}
	if (direction & KEYINPUT::BACKWARD)
	{
		XMStoreFloat3(&shift, XMLoadFloat3(&shift) + (XMLoadFloat3(&look) * -max_speed));
	}
	if (direction & KEYINPUT::RIGHT)
	{
		XMStoreFloat3(&shift, XMLoadFloat3(&shift) + (XMLoadFloat3(&right) * -max_speed));
	}
	if (direction & KEYINPUT::LEFT)
	{
		XMStoreFloat3(&shift, XMLoadFloat3(&shift) + (XMLoadFloat3(&right) * max_speed));		
	}

	//x += temp_x;
	//z += temp_z;

	XMFLOAT3 position{ x, y, z };
	XMStoreFloat3(&position, XMLoadFloat3(&position) + XMLoadFloat3(&shift));
	
	x = position.x;
	y = position.y;
	z = position.z;

	// 디버깅 용 플레이어 좌표 출력 문구
	//std::cout << x << ", " << z << std::endl;
}

void Player::Rotate(float cy)
{
	if (cy != 0.0f)
	{
		XMMATRIX rotate{ XMMatrixRotationAxis(XMLoadFloat3(&up), XMConvertToRadians(cy)) };
		XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), rotate));
		XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), rotate));
	}

	XMStoreFloat3(&look, XMVector3Normalize(XMLoadFloat3(&look)));
	XMStoreFloat3(&right, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&up), XMLoadFloat3(&look))));
	XMStoreFloat3(&right, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&look), XMLoadFloat3(&right))));
}

void Player::Attack(int interaction)
{
}
