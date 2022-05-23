#pragma once

extern void AnimateFbxMesh(FbxMesh *pfbxMesh, FbxTime& fbxCurrentTime);
extern void AnimateFbxNodeHierarchy(FbxNode *pfbxNode, FbxTime& fbxCurrentTime);

extern void RenderFbxMesh(ID3D12GraphicsCommandList *pd3dCommandList, FbxMesh *pfbxMesh, FbxAMatrix& fbxmtxNodeToRoot, FbxAMatrix& fbxmtxGeometryOffset, FbxAMatrix fbxmtxWorld);
extern void RenderFbxNodeHierarchy(ID3D12GraphicsCommandList *pd3dCommandList, FbxNode *pfbxNode, FbxTime& fbxCurrentTime, FbxAMatrix& fbxmtxWorld);

extern void CreateMeshFromFbxNodeHierarchy(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, FbxNode *pfbxNode);
extern void ReleaseMeshFromFbxNodeHierarchy(FbxNode *pfbxNode);
extern void ReleaseUploadBufferFromFbxNodeHierarchy(FbxNode *pfbxNode);

extern FbxScene *LoadFbxSceneFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, FbxManager *pfbxSdkManager, char *pstrFbxFileName);

extern XMFLOAT4X4 FbxMatrixToXmFloat4x4Matrix(FbxAMatrix *pfbxmtxSource);
extern FbxAMatrix XmFloat4x4MatrixToFbxMatrix(XMFLOAT4X4& xmf4x4Source);

