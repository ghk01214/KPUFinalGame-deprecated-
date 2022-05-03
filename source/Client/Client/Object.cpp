#include "pch.h"
#include "Object.h"
#include "Shader.h"

CGameObject::CGameObject(int nMeshes)
{
	m_xmf4x4World = Matrix4x4::Identity();
	m_nMeshes = nMeshes;
	m_ppMeshes = nullptr;
	if (m_nMeshes > 0)
	{
		m_ppMeshes = new CMesh * [m_nMeshes];
		for (int i = 0; i < m_nMeshes; i++) m_ppMeshes[i] = nullptr;
	}
}

CGameObject::~CGameObject()
{
	if (m_ppMeshes) {
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = nullptr;
		}
		delete[] m_ppMeshes;
	}
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}

void CGameObject::SetMesh(int nIndex, CMesh* pMesh)
{
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();
		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
}

void CGameObject::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CGameObject::Animate(float fTimeElapsed)
{
	UpdateBoundingBox();
}

void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 3, &m_xmf3Color, 16);
}

void CGameObject::ReleaseShaderVariables()
{
}

void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();

	if (m_pShader)
	{
		m_pShader->Render(pd3dCommandList, pCamera);
	}

	UpdateShaderVariables(pd3dCommandList);

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}
}

void CGameObject::UpdateBoundingBox()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; ++i) {
			m_ppMeshes[i]->GetBoundingBox().Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
			XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
		}
	}
}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->ReleaseUploadBuffers();
		}
	}
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::SetLook(XMFLOAT3 xmf3Look)
{
	m_xmf4x4World._31 = xmf3Look.x;
	m_xmf4x4World._32 = xmf3Look.y;
	m_xmf4x4World._33 = xmf3Look.z;
}


void CGameObject::SetUp(XMFLOAT3 xmf3Up)
{
	m_xmf4x4World._21 = xmf3Up.x;
	m_xmf4x4World._22 = xmf3Up.y;
	m_xmf4x4World._23 = xmf3Up.z;
}

void CGameObject::SetRight(XMFLOAT3 xmf3Right)
{
	m_xmf4x4World._11 = xmf3Right.x;
	m_xmf4x4World._12 = xmf3Right.y;
	m_xmf4x4World._13 = xmf3Right.z;
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}

XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}

void CGameObject::SetScale(float scale)
{
	m_xmf4x4World._11 = scale;
	m_xmf4x4World._22 = scale;
	m_xmf4x4World._33 = scale;
}

void CGameObject::MoveStrafe(float fDistance)
{

	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveBack(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CUfoObject::CUfoObject(int num)
{
	type = num;
}

CUfoObject::~CUfoObject()
{

}

void CUfoObject::Animate(float fTimeElapsed)
{
	if (type == 1) //우상
	{
		CGameObject::MoveForward(m_fMovingSpeed + m_ObjectsAcceleration);  // 세로
		CGameObject::MoveStrafe(m_fMovingSpeed + m_ObjectsAcceleration);   // 가로
		m_ObjectsAcceleration += (fTimeElapsed * 0.1f);
	}
	else if (type == 2) //좌상
	{
		CGameObject::MoveForward(m_fMovingSpeed + m_ObjectsAcceleration);  // 세로
		CGameObject::MoveStrafe(-(m_fMovingSpeed + m_ObjectsAcceleration));   // 가로 
		m_ObjectsAcceleration += (fTimeElapsed * 0.1f);
	}
	else if (type == 3) //좌하
	{
		CGameObject::MoveForward(-(m_fMovingSpeed + m_ObjectsAcceleration));  // 세로
		CGameObject::MoveStrafe(-(m_fMovingSpeed + m_ObjectsAcceleration));   // 가로 
		m_ObjectsAcceleration += (fTimeElapsed * 0.1f);
	}
	else if (type == 4) //우하
	{
		CGameObject::MoveForward(-(m_fMovingSpeed + m_ObjectsAcceleration));  // 세로
		CGameObject::MoveStrafe(m_fMovingSpeed + m_ObjectsAcceleration);   // 가로 
		m_ObjectsAcceleration += (fTimeElapsed * 0.1f);
	}
	else if (type == 5)
	{
		CGameObject::MoveForward(m_fMovingSpeed + m_ObjectsAcceleration);  // 세로
		m_ObjectsAcceleration += (fTimeElapsed * 0.1f);
	}

	else if (type == 6)
	{

		CGameObject::MoveStrafe(-(m_fMovingSpeed + m_ObjectsAcceleration));   // 가로 
		m_ObjectsAcceleration += (fTimeElapsed * 0.1f);
	}

	else if (type == 7)
	{
		CGameObject::MoveForward(-(m_fMovingSpeed + m_ObjectsAcceleration));  // 세로
		m_ObjectsAcceleration += (fTimeElapsed * 0.1f);
	}

	else if (type == 8)
	{
		CGameObject::MoveStrafe(m_fMovingSpeed + m_ObjectsAcceleration);   // 가로
		m_ObjectsAcceleration += (fTimeElapsed * 0.25f);
	}
	if (!Hit)
	{
		Rotate(720.0f * fTimeElapsed, 720.0f * fTimeElapsed, -720.0f * fTimeElapsed);
		MoveUp(1500.0f * fTimeElapsed);
	}
	CGameObject::Animate(fTimeElapsed);
}

CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int
	nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4
	xmf4Color) : CGameObject(0)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	m_xmf3Scale = xmf3Scale;

	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (m_nLength - 1) / czQuadsPerBlock;

	m_nMeshes = cxBlocks * czBlocks;

	m_ppMeshes = new CMesh * [m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)m_ppMeshes[i] = nullptr;

	CHeightMapGridMesh* pHeightMapGridMesh = nullptr;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			xStart = x * (nBlockWidth - 1);
			zStart = z * (nBlockLength - 1);
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, xStart,
				zStart, nBlockWidth, nBlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(x + (z * cxBlocks), pHeightMapGridMesh);
		}
	}
	//CPseudoLightingShader* pShader = new CPseudoLightingShader();
	CTerrainShader* pShader = new CTerrainShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	SetShader(pShader);
}

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}

CMissileObject::CMissileObject()
{
}

CMissileObject::~CMissileObject()
{
}

void CMissileObject::Animate(float fTimeElapsed)
{
	if (m_fire)
	{
		MoveForward(1800.0f * fTimeElapsed);
	}
	ResetPosition();
	UpdateBoundingBox();
}

void CMissileObject::ResetPosition()
{
	if (m_xmf4x4World._41 < 0 || m_xmf4x4World._41 > 2500) {
		SetPosition(0.0f, -1000.0f, 0.0f);
		SetFire(false);
	}
	else if (m_xmf4x4World._43 < 0 || m_xmf4x4World._43 > 2500) {
		SetPosition(0.0f, -1000.0f, 0.0f);
		SetFire(false);
	}
}

CBuildingObject::CBuildingObject()
{
}

CBuildingObject::~CBuildingObject()
{
}
