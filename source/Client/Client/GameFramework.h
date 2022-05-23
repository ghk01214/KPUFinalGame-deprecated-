#pragma once

#define FRAME_BUFFER_WIDTH		640
#define FRAME_BUFFER_HEIGHT		480

#include "Timer.h"
#include "Player.h"
#include "Scene.h"
#include "Network.h"
#include "../../Server/Server/protocol.hpp"

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	// 게임 프레임워크 복사 방지
	CGameFramework(const CGameFramework& other) = delete;
	CGameFramework& operator=(const CGameFramework& other) = delete;

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();

	void CreateRtvAndDsvDescriptorHeaps();

	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void ChangeSwapChainState();

<<<<<<< HEAD
	void ConnectToServer() { network_manager->ConnectToServer(); }
	void Login() { network_manager->SendLoginPacket(); }
	void RecvData() { network_manager->RecvData(); }
	void AddPlayer(SC::PACKET::ADD_PLAYER* packet = nullptr);

	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();
=======
	void AddPlayer(unsigned short id, short x, short y, short z);
    void BuildObjects();
    void ReleaseObjects();
>>>>>>> Player

    void ProcessInput();
    void AnimateObjects();
    void FrameAdvance();

	void WaitForGpuComplete();
	void MoveToNextFrame();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	CPlayer* GetPlayer() { return m_pPlayer; }
	CScene* GetScene() { return m_pScene; }
	CGameTimer* GetTimer() { return &m_GameTimer; }

private:
	HINSTANCE					m_hInstance;
	HWND						m_hWnd; 

	int							m_nWndClientWidth;
	int							m_nWndClientHeight;
        
	IDXGIFactory4				*m_pdxgiFactory = NULL;
	IDXGISwapChain3				*m_pdxgiSwapChain = NULL;
	ID3D12Device				*m_pd3dDevice = NULL;

	bool						m_bMsaa4xEnable = false;
	UINT						m_nMsaa4xQualityLevels = 0;

	static const UINT			m_nSwapChainBuffers = 2;
	UINT						m_nSwapChainBufferIndex;

	ID3D12Resource				*m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap		*m_pd3dRtvDescriptorHeap = NULL;
	UINT						m_nRtvDescriptorIncrementSize;

	ID3D12Resource				*m_pd3dDepthStencilBuffer = NULL;
	ID3D12DescriptorHeap		*m_pd3dDsvDescriptorHeap = NULL;
	UINT						m_nDsvDescriptorIncrementSize;

	ID3D12CommandAllocator		*m_pd3dCommandAllocator = NULL;
	ID3D12CommandQueue			*m_pd3dCommandQueue = NULL;
	ID3D12GraphicsCommandList	*m_pd3dCommandList = NULL;

	ID3D12Fence					*m_pd3dFence = NULL;
	UINT64						m_nFenceValues[m_nSwapChainBuffers];
	HANDLE						m_hFenceEvent;

<<<<<<< HEAD
#ifndef _DEBUG
	ID3D12Debug* m_pd3dDebugController;
=======
#if defined(_DEBUG)
	ID3D12Debug					*m_pd3dDebugController;
>>>>>>> Player
#endif

	CGameTimer					m_GameTimer;

	CScene						*m_pScene = NULL;
	CPlayer						*m_pPlayer = NULL;
	CPlayer* m_pPlayer2 = NULL;
	CPlayer* m_pPlayer3 = NULL;
	CPlayer* m_pPlayer4 = NULL;

	std::unordered_map<int, CPlayer*> players;

	CCamera						*m_pCamera = NULL;

	POINT						m_ptOldCursorPos;

<<<<<<< HEAD
	CGameTimer					m_GameTimer;
	_TCHAR						m_pszFrameRate[50];
=======
	_TCHAR						m_pszFrameRate[70];
};
>>>>>>> Player

	std::unique_ptr<CNetwork> network_manager;
};
