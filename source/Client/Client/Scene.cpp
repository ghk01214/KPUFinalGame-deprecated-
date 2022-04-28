//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

// 좌표 찾기
// 디펜스 게임
// 도시 느낌 나게 건물 배치

#include "pch.h"
#include "Player.h"
#include "Scene.h"

CScene::CScene()
{
}

CScene::~CScene()
{
}

//#define _WITH_TEXT_MODEL_FILE
#define _WITH_BINARY_MODEL_FILE

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	std::uniform_int_distribution<int>rd(0, 255);
	std::default_random_engine dre;

#ifdef _WITH_TEXT_MODEL_FILE
	CMesh* pUfoMesh = new CMesh(pd3dDevice, pd3dCommandList, "Models/UFO.txt", true);
	CMesh* pFlyerMesh = new CMesh(pd3dDevice, pd3dCommandList, "Models/FlyerPlayership.txt", true);
#endif

#ifdef _WITH_BINARY_MODEL_FILE
	CMesh* pUfoMesh = new CMesh(pd3dDevice, pd3dCommandList, "Assets/Models/UFO.bin", false);
	CMesh* pHouseMesh = new CMesh(pd3dDevice, pd3dCommandList, "Assets/Models/ams_house3.bin", false);
	CMesh* pFlyerMesh = new CMesh(pd3dDevice, pd3dCommandList, "Assets/Models/FlyerPlayership.bin", false);
#endif
	//지형을 확대할 스케일 벡터이다. x-축과 z-축은 8배, y-축은 2배 확대한다. 
	XMFLOAT3 xmf3Scale(9.0f, 1.0f, 9.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.2f, 0.0f, 0.0f);
	//지형을 높이 맵 이미지 파일(HeightMap.raw)을 사용하여 생성한다. 높이 맵의 크기는 가로x세로(257x257)이다. 
#ifdef _WITH_TERRAIN_PARTITION
	/*하나의 격자 메쉬의 크기는 가로x세로(17x17)이다. 지형 전체는 가로 방향으로 16개, 세로 방향으로 16의 격자 메
	쉬를 가진다. 지형을 구성하는 격자 메쉬의 개수는 총 256(16x16)개가 된다.*/
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, _T("../Assets/Image/Terrain/HeightMap.raw"), 257, 257, 17,
		17, xmf3Scale, xmf4Color);
#else
//지형을 하나의 격자 메쉬(257x257)로 생성한다. 
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, _T("Assets/Image/Terrain/HeightMap.raw"), 257, 257, 257,
		257, xmf3Scale, xmf4Color);
#endif

	m_nObjects = 8;
	m_ppObjects = new CGameObject * [m_nObjects];

	CPseudoLightingShader* pShader = new CPseudoLightingShader();
	pShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//ufo 를 한 4셋트 정도 제작 점점 크기가 작어져서 난이도 상승
	//1번 세트
	{
		m_ppObjects[0] = new CUfoObject(1);
		m_ppObjects[0]->SetMesh(0, pUfoMesh);
		m_ppObjects[0]->SetShader(pShader);
		m_ppObjects[0]->SetScale(40.0f);
		m_ppObjects[0]->SetPosition(0.0f, m_pTerrain->GetHeight(100.0f, 100.0f), 0.0f);
		m_ppObjects[0]->SetColor(XMFLOAT3(200.0f, 0.0f, 0.0f));

		m_ppObjects[1] = new CUfoObject(2);
		m_ppObjects[1]->SetMesh(0, pUfoMesh);
		m_ppObjects[1]->SetShader(pShader);
		m_ppObjects[1]->SetScale(40.0f);
		m_ppObjects[1]->SetPosition(2400.0f, m_pTerrain->GetHeight(100.0f, 100.0f), 0.0f);
		m_ppObjects[1]->SetColor(XMFLOAT3(200.0f, 0.0f, 0.0f));

		m_ppObjects[2] = new CUfoObject(3);
		m_ppObjects[2]->SetMesh(0, pUfoMesh);
		m_ppObjects[2]->SetShader(pShader);
		m_ppObjects[2]->SetScale(40.0f);
		m_ppObjects[2]->SetPosition(2400.0f, m_pTerrain->GetHeight(100.0f, 100.0f), 2400.0f);
		m_ppObjects[2]->SetColor(XMFLOAT3(200.0f, 0.0f, 0.0f));

		m_ppObjects[3] = new CUfoObject(4);
		m_ppObjects[3]->SetMesh(0, pUfoMesh);
		m_ppObjects[3]->SetShader(pShader);
		m_ppObjects[3]->SetScale(40.0f);
		m_ppObjects[3]->SetPosition(0.0f, m_pTerrain->GetHeight(100.0f, 100.0f), 2400.0f);
		m_ppObjects[3]->SetColor(XMFLOAT3(200.0f, 0.0f, 0.0f));

		m_ppObjects[4] = new CUfoObject(5);
		m_ppObjects[4]->SetMesh(0, pUfoMesh);
		m_ppObjects[4]->SetShader(pShader);
		m_ppObjects[4]->SetScale(40.0f);
		m_ppObjects[4]->SetPosition(1150.0f, m_pTerrain->GetHeight(100.0f, 100.0f), 0.0f);
		m_ppObjects[4]->SetColor(XMFLOAT3(200.0f, 0.0f, 0.0f));

		m_ppObjects[5] = new CUfoObject(6);
		m_ppObjects[5]->SetMesh(0, pUfoMesh);
		m_ppObjects[5]->SetShader(pShader);
		m_ppObjects[5]->SetScale(40.0f);
		m_ppObjects[5]->SetPosition(2400.0f, m_pTerrain->GetHeight(100.0f, 100.0f), 1150.0f);
		m_ppObjects[5]->SetColor(XMFLOAT3(200.0f, 0.0f, 0.0f));

		m_ppObjects[6] = new CUfoObject(7);
		m_ppObjects[6]->SetMesh(0, pUfoMesh);
		m_ppObjects[6]->SetShader(pShader);
		m_ppObjects[6]->SetScale(40.0f);
		m_ppObjects[6]->SetPosition(1150.0f, m_pTerrain->GetHeight(100.0f, 100.0f), 2400.0f);
		m_ppObjects[6]->SetColor(XMFLOAT3(200.0f, 0.0f, 0.0f));

		m_ppObjects[7] = new CUfoObject(8);
		m_ppObjects[7]->SetMesh(0, pUfoMesh);
		m_ppObjects[7]->SetShader(pShader);
		m_ppObjects[7]->SetScale(40.0f);
		m_ppObjects[7]->SetPosition(0.0f, m_pTerrain->GetHeight(100.0f, 100.0f), 1150.0f);
		m_ppObjects[7]->SetColor(XMFLOAT3(200.0f, 0.0f, 0.0f));
	}
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = nullptr;

	D3D12_ROOT_PARAMETER pd3dRootParameters[3];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 4; //Time, ElapsedTime, xCursor, yCursor
	pd3dRootParameters[0].Constants.ShaderRegister = 0; //Time
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 19; //16 + 3
	pd3dRootParameters[1].Constants.ShaderRegister = 1; //World
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[2].Constants.Num32BitValues = 35; //16 + 16 + 3
	pd3dRootParameters[2].Constants.ShaderRegister = 2; //Camera
	pd3dRootParameters[2].Constants.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = nullptr;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = nullptr;
	ID3DBlob* pd3dErrorBlob = nullptr;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) delete m_ppObjects[j];
		delete[] m_ppObjects;
	}

	if (m_pTerrain) delete m_pTerrain;
}

void CScene::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->ReleaseUploadBuffers();
	}

	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
}

void CScene::CheckPlayerByObjectCollisions()
{
	CTerrainPlayer* Player = (CTerrainPlayer*)m_pPlayer;

	for (int i = 0; i < m_nObjects; i++) {
		if (m_ppObjects[i]->Hit)
			if (m_ppObjects[i]->m_xmOOBB.Intersects(Player->m_xmOOBB))
				Player->ResetPlayerPos();
	}
}

void CScene::CheckMissileByObjectCollisions()
{
	CTerrainPlayer* Player = (CTerrainPlayer*)m_pPlayer;

	for (int i = 0; i < m_nObjects; i++)
		if (m_ppObjects[i]->Hit)
		{
			for (int j = 0; j < Player->GetMissileNum(); ++j)
			{
				CMissileObject* Missile = Player->GetMissile(j);
				if (Missile->GetFire())
					if (m_ppObjects[i]->m_xmOOBB.Intersects(Missile->m_xmOOBB))
					{
						Missile->SetFire(false);
						Missile->SetPosition(0.0f, -1000.0f, 0.0f);
						m_ppObjects[i]->Hit = false;
					}
			}
		}
}

void CScene::CheckMissileByTerrainCollisions()
{
	CTerrainPlayer* Player = (CTerrainPlayer*)m_pPlayer;
	for (int i = 0; i < Player->GetMissileNum(); ++i) {
		CMissileObject* Missile = Player->GetMissile(i);
		XMFLOAT3 posi = Missile->GetPosition();
		if (Missile->GetFire() && m_pTerrain->GetHeight(posi.x, posi.z) > posi.y) {
   			Missile->SetFire(false);
			Missile->SetPosition(0.0f, -1000.0f, 0.0f);
		}
	}
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::ProcessInput()
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);
	}
	//CheckPlayerByObjectCollisions();
	CheckMissileByTerrainCollisions();
	CheckMissileByObjectCollisions();
}

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	if (m_pTerrain)
		m_pTerrain->Render(pd3dCommandList, pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) m_ppObjects[j]->Render(pd3dCommandList, pCamera);
	}
}

