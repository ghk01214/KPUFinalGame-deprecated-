#pragma once

class CVertex
{
protected:
	XMFLOAT3						m_xmf3Position;

public:
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~CVertex() { }
};

class CDiffusedVertex : public CVertex
{
protected:
	XMFLOAT4						m_xmf4Diffuse;

public:
	CDiffusedVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f); }
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse) { m_xmf3Position = XMFLOAT3(x, y, z); m_xmf4Diffuse = xmf4Diffuse; }
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse) { m_xmf3Position = xmf3Position; m_xmf4Diffuse = xmf4Diffuse; }
	~CDiffusedVertex() { }
};

class CMesh
{
public:
	CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName = nullptr, bool bTextFile = true);
	virtual ~CMesh();

private:
	int								m_nReferences = 0;

public:
	void AddRef() { ++m_nReferences; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void ReleaseUploadBuffers();

	BoundingOrientedBox GetBoundingBox();

protected:
	BoundingOrientedBox				m_xmBoundingBox;

	UINT							m_nVertices = 0;
	XMFLOAT3* m_pxmf3Positions = nullptr;
	ID3D12Resource* m_pd3dPositionBuffer = nullptr;
	ID3D12Resource* m_pd3dPositionUploadBuffer = nullptr;

	XMFLOAT3* m_pxmf3Normals = nullptr;
	ID3D12Resource* m_pd3dNormalBuffer = nullptr;
	ID3D12Resource* m_pd3dNormalUploadBuffer = nullptr;

	XMFLOAT2* m_pxmf2TextureCoords = nullptr;
	ID3D12Resource* m_pd3dTextureCoordBuffer = nullptr;
	ID3D12Resource* m_pd3dTextureCoordUploadBuffer = nullptr;

	UINT							m_nIndices = 0;
	UINT* m_pnIndices = nullptr;
	ID3D12Resource* m_pd3dIndexBuffer = nullptr;
	ID3D12Resource* m_pd3dIndexUploadBuffer = nullptr;

	ID3D12Resource* m_pd3dVertexBuffer = nullptr;
	ID3D12Resource* m_pd3dVertexUploadBuffer = nullptr;

	UINT							m_nVertexBufferViews = 0;
	D3D12_VERTEX_BUFFER_VIEW* m_pd3dVertexBufferViews = nullptr;

	D3D12_INDEX_BUFFER_VIEW			m_d3dIndexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT							m_nSlot = 0;
	UINT							m_nStride = 0;
	UINT							m_nOffset = 0;

	UINT							m_nStartIndex = 0;
	int								m_nBaseVertex = 0;

public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);

	void LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName, bool bTextFile);
};

class CHeightMapImage
{
private:
	BYTE* m_pHeightMapPixels;
 
	int m_nWidth;
	int m_nLength;

XMFLOAT3 m_xmf3Scale; public:

	CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	~CHeightMapImage();

	float GetHeight(float x, float z);

	XMFLOAT3 GetHeightMapNormal(int x, int z);

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	BYTE* GetHeightMapPixels() { return(m_pHeightMapPixels); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }
};

class CHeightMapGridMesh : public CMesh
{
protected:
	int m_nWidth;
	int m_nLength;

	XMFLOAT3 m_xmf3Scale;
public:
	CHeightMapGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList, int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale =
		XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), void
		* pContext = nullptr);
	virtual ~CHeightMapGridMesh();

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	int GetWidth() { return(m_nWidth); }
	int GetLength() { return(m_nLength); }

	virtual float OnGetHeight(int x, int z, void* pContext);
 
	virtual XMFLOAT4 OnGetColor(int x, int z, void* pContext);
};