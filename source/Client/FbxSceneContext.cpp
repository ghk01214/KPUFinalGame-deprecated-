#include "pch.h"
#include "Object.h"

XMFLOAT4X4 FbxMatrixToXmFloat4x4Matrix(FbxAMatrix *pfbxmtxSource)
{
	XMFLOAT4X4 xmf4x4Result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) xmf4x4Result.m[i][j] = (float)(*pfbxmtxSource)[i][j];
//		for (int j = 0; j < 4; j++) xmf4x4Result.m[i][j] = (float)(*pfbxmtxSource)[j][i];
	}
	return(xmf4x4Result);
}

FbxAMatrix XmFloat4x4MatrixToFbxMatrix(XMFLOAT4X4& xmf4x4Source)
{
	FbxAMatrix fbxmtxResult;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) fbxmtxResult[i][j] = xmf4x4Source.m[i][j];
	}
	return(fbxmtxResult);
}

void MatrixScale(FbxAMatrix& fbxmtxSrcMatrix, double pValue)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) fbxmtxSrcMatrix[i][j] *= pValue;
	}
}

void MatrixAddToDiagonal(FbxAMatrix& fbxmtxSrcMatrix, double pValue)
{
	fbxmtxSrcMatrix[0][0] += pValue;
	fbxmtxSrcMatrix[1][1] += pValue;
	fbxmtxSrcMatrix[2][2] += pValue;
	fbxmtxSrcMatrix[3][3] += pValue;
}

void MatrixAdd(FbxAMatrix& fbxmtxDstMatrix, FbxAMatrix& fbxmtxSrcMatrix)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) fbxmtxDstMatrix[i][j] += fbxmtxSrcMatrix[i][j];
	}
}

FbxAMatrix GetGeometricTransform(FbxNode *pfbxNode)
{
	const FbxVector4 T = pfbxNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 R = pfbxNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 S = pfbxNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return(FbxAMatrix(T, R, S));
}

FbxAMatrix ComputeClusterDeformation(FbxMesh *pfbxMesh, FbxCluster *pfbxCluster, FbxTime& fbxCurrentTime)
{
	FbxAMatrix fbxmtxVertexTransform;

    FbxCluster::ELinkMode nClusterMode = pfbxCluster->GetLinkMode();
	if (nClusterMode == FbxCluster::eNormalize) 
	{
		FbxAMatrix fbxmtxGeometryOffset = GetGeometricTransform(pfbxMesh->GetNode());

		FbxAMatrix fbxmtxClusterTransform;
		pfbxCluster->GetTransformMatrix(fbxmtxClusterTransform);

		FbxAMatrix fbxmtxClusterLinkTransform;
		pfbxCluster->GetTransformLinkMatrix(fbxmtxClusterLinkTransform);

		FbxAMatrix fbxmtxLinkNodeToRoot = pfbxCluster->GetLink()->EvaluateGlobalTransform(fbxCurrentTime);

		fbxmtxVertexTransform = fbxmtxLinkNodeToRoot * fbxmtxClusterLinkTransform.Inverse() * fbxmtxClusterTransform * fbxmtxGeometryOffset;
	}
	else
	{ //FbxCluster::eAdditive
		if (pfbxCluster->GetAssociateModel())
		{
			FbxAMatrix fbxmtxAssociateModel;
			pfbxCluster->GetTransformAssociateModelMatrix(fbxmtxAssociateModel);

			FbxAMatrix fbxmtxAssociateGeometryOffset = GetGeometricTransform(pfbxCluster->GetAssociateModel());
			fbxmtxAssociateModel *= fbxmtxAssociateGeometryOffset;

			FbxAMatrix fbxmtxAssociateModelGlobal = pfbxCluster->GetAssociateModel()->EvaluateGlobalTransform(fbxCurrentTime);

			FbxAMatrix fbxmtxClusterTransform;
			pfbxCluster->GetTransformMatrix(fbxmtxClusterTransform);

			FbxAMatrix fbxmtxGeometryOffset = GetGeometricTransform(pfbxMesh->GetNode());
			fbxmtxClusterTransform *= fbxmtxGeometryOffset;

			FbxAMatrix fbxmtxClusterLinkTransform;
			pfbxCluster->GetTransformLinkMatrix(fbxmtxClusterLinkTransform);

			FbxAMatrix fbxmtxLinkGeometryOffset = GetGeometricTransform(pfbxCluster->GetLink());
			fbxmtxClusterLinkTransform *= fbxmtxLinkGeometryOffset;

			FbxAMatrix fbxmtxClusterLinkToRoot = pfbxCluster->GetLink()->EvaluateGlobalTransform(fbxCurrentTime);

			fbxmtxVertexTransform = fbxmtxClusterTransform.Inverse() * fbxmtxAssociateModel * fbxmtxAssociateModelGlobal.Inverse() * fbxmtxClusterLinkToRoot * fbxmtxClusterLinkTransform.Inverse() * fbxmtxClusterTransform;
		}
	}
	return(fbxmtxVertexTransform);
}

void ComputeLinearDeformation(FbxMesh *pfbxMesh, FbxTime& fbxCurrentTime, FbxVector4 *pfbxv4Vertices, int nVertices)
{
	FbxAMatrix *pfbxmtxClusterDeformations = new FbxAMatrix[nVertices];
	::memset(pfbxmtxClusterDeformations, 0, nVertices * sizeof(FbxAMatrix));

	double *pfSumOfClusterWeights = new double[nVertices];
	::memset(pfSumOfClusterWeights, 0, nVertices * sizeof(double));

	FbxCluster::ELinkMode nClusterMode = ((FbxSkin *)pfbxMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();
	if (nClusterMode == FbxCluster::eAdditive)
	{
		for (int i = 0; i < nVertices; ++i) pfbxmtxClusterDeformations[i].SetIdentity();
	}

	int nSkinDeformers = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int i = 0; i < nSkinDeformers; i++)
	{
		FbxSkin *pfbxSkinDeformer = (FbxSkin *)pfbxMesh->GetDeformer(i, FbxDeformer::eSkin);	
		int nClusters = pfbxSkinDeformer->GetClusterCount();

		for (int j = 0; j < nClusters; j++)
		{
			FbxCluster *pfbxCluster = pfbxSkinDeformer->GetCluster(j);
			if (!pfbxCluster->GetLink()) continue;

			FbxAMatrix fbxmtxClusterDeformation = ComputeClusterDeformation(pfbxMesh, pfbxCluster, fbxCurrentTime);

			int *pnIndices = pfbxCluster->GetControlPointIndices();
			double *pfWeights = pfbxCluster->GetControlPointWeights();

			int nIndices = pfbxCluster->GetControlPointIndicesCount();
			for (int k = 0; k < nIndices; k++)
			{            
				int nIndex = pnIndices[k];
				double fWeight = pfWeights[k];
				if ((nIndex >= nVertices) || (fWeight == 0.0)) continue;

				FbxAMatrix fbxmtxInfluence = fbxmtxClusterDeformation;
				MatrixScale(fbxmtxInfluence, fWeight);

				if (nClusterMode == FbxCluster::eAdditive)
				{    
					MatrixAddToDiagonal(fbxmtxInfluence, 1.0 - fWeight);
					pfbxmtxClusterDeformations[nIndex] = fbxmtxInfluence * pfbxmtxClusterDeformations[nIndex];
					pfSumOfClusterWeights[nIndex] = 1.0;
				}
				else 
				{
					MatrixAdd(pfbxmtxClusterDeformations[nIndex], fbxmtxInfluence);
					pfSumOfClusterWeights[nIndex] += fWeight;
				}
			}			
		}
	}

	for (int i = 0; i < nVertices; i++) 
	{
		if (pfSumOfClusterWeights[i] != 0.0)
		{
			FbxVector4 fbxv4Vertex = pfbxv4Vertices[i];
			pfbxv4Vertices[i] = pfbxmtxClusterDeformations[i].MultT(fbxv4Vertex);
			if (nClusterMode == FbxCluster::eNormalize)
			{
				pfbxv4Vertices[i] /= pfSumOfClusterWeights[i];
			}
			else if (nClusterMode == FbxCluster::eTotalOne)
			{
				fbxv4Vertex *= (1.0 - pfSumOfClusterWeights[i]);
				pfbxv4Vertices[i] += fbxv4Vertex;
			}
		} 
	}

	delete[] pfbxmtxClusterDeformations;
	delete[] pfSumOfClusterWeights;
}

void ComputeDualQuaternionDeformation(FbxMesh *pfbxMesh, FbxTime& fbxCurrentTime, FbxVector4 *pfbxv4Vertices, int nVertices)
{
	FbxDualQuaternion *pfbxDQClusterDeformations = new FbxDualQuaternion[nVertices];
	memset(pfbxDQClusterDeformations, 0, nVertices * sizeof(FbxDualQuaternion));
	double *pfClusterWeights = new double[nVertices];
	memset(pfClusterWeights, 0, nVertices * sizeof(double));

	FbxCluster::ELinkMode nClusterMode = ((FbxSkin *)pfbxMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();
	int nSkinDeformers = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int i = 0; i < nSkinDeformers; i++)
	{
		FbxSkin *pfbxSkinDeformer = (FbxSkin *)pfbxMesh->GetDeformer(i, FbxDeformer::eSkin);
		int nClusters = pfbxSkinDeformer->GetClusterCount();
		for (int j = 0; j < nClusters; j++)
		{
			FbxCluster *pfbxCluster = pfbxSkinDeformer->GetCluster(j);
			if (!pfbxCluster->GetLink()) continue;

			FbxAMatrix fbxmtxCluster = ComputeClusterDeformation(pfbxMesh, pfbxCluster, fbxCurrentTime);

			FbxQuaternion Q = fbxmtxCluster.GetQ();
			FbxVector4 T = fbxmtxCluster.GetT();
			FbxDualQuaternion fbxDualQuaternion(Q, T);

			int nIndices = pfbxCluster->GetControlPointIndicesCount();
			int *pnControlPointIndices = pfbxCluster->GetControlPointIndices();
			double *pfControlPointWeights = pfbxCluster->GetControlPointWeights();
			for (int k = 0; k < nIndices; ++k) 
			{ 
				int nIndex = pnControlPointIndices[k];
				if (nIndex >= nVertices) continue;

				double fWeight = pfControlPointWeights[k];
				if (fWeight == 0.0) continue;

				FbxDualQuaternion fbxmtxInfluence = fbxDualQuaternion * fWeight;
				if (nClusterMode == FbxCluster::eAdditive)
				{    
					pfbxDQClusterDeformations[nIndex] = fbxmtxInfluence;
					pfClusterWeights[nIndex] = 1.0;
				}
				else // FbxCluster::eNormalize || FbxCluster::eTotalOne
				{
					if (j == 0)
					{
						pfbxDQClusterDeformations[nIndex] = fbxmtxInfluence;
					}
					else
					{
						double fSign = pfbxDQClusterDeformations[nIndex].GetFirstQuaternion().DotProduct(fbxDualQuaternion.GetFirstQuaternion());
						if (fSign >= 0.0 )
						{
							pfbxDQClusterDeformations[nIndex] += fbxmtxInfluence;
						}
						else
						{
							pfbxDQClusterDeformations[nIndex] -= fbxmtxInfluence;
						}
					}
					pfClusterWeights[nIndex] += fWeight;
				}
			}
		}
	}

	for (int i = 0; i < nVertices; i++) 
	{
		FbxVector4 fbxv4SrcVertex = pfbxv4Vertices[i];
		double fWeightSum = pfClusterWeights[i];

		if (fWeightSum != 0.0) 
		{
			pfbxDQClusterDeformations[i].Normalize();
			pfbxv4Vertices[i] = pfbxDQClusterDeformations[i].Deform(pfbxv4Vertices[i]);

			if (nClusterMode == FbxCluster::eNormalize)
			{
				pfbxv4Vertices[i] /= fWeightSum;
			}
			else if (nClusterMode == FbxCluster::eTotalOne)
			{
				fbxv4SrcVertex *= (1.0 - fWeightSum);
				pfbxv4Vertices[i] += fbxv4SrcVertex;
			}
		} 
	}

	delete[] pfbxDQClusterDeformations;
	delete[] pfClusterWeights;
}

void ComputeSkinDeformation(FbxMesh *pfbxMesh, FbxTime& fbxCurrentTime, FbxVector4 *pfbxv4Vertices, int nVertices)
{
	FbxSkin *pfbxSkinDeformer = (FbxSkin *)pfbxMesh->GetDeformer(0, FbxDeformer::eSkin);
	FbxSkin::EType nSkinningType = pfbxSkinDeformer->GetSkinningType();

	if ((nSkinningType == FbxSkin::eLinear) || (nSkinningType == FbxSkin::eRigid))
	{
		ComputeLinearDeformation(pfbxMesh, fbxCurrentTime, pfbxv4Vertices, nVertices);
	}
	else if (nSkinningType == FbxSkin::eDualQuaternion)
	{
		ComputeDualQuaternionDeformation(pfbxMesh, fbxCurrentTime, pfbxv4Vertices, nVertices);
	}
	else if (nSkinningType == FbxSkin::eBlend)
	{
		FbxVector4 *pfbxv4LinearVertices = new FbxVector4[nVertices];
		memcpy(pfbxv4LinearVertices, pfbxMesh->GetControlPoints(), nVertices * sizeof(FbxVector4));
		ComputeLinearDeformation(pfbxMesh, fbxCurrentTime, pfbxv4LinearVertices, nVertices);

		FbxVector4 *pfbxv4DQVertices = new FbxVector4[nVertices];
		memcpy(pfbxv4DQVertices, pfbxMesh->GetControlPoints(), nVertices * sizeof(FbxVector4));
		ComputeDualQuaternionDeformation(pfbxMesh, fbxCurrentTime, pfbxv4DQVertices, nVertices);

		int nBlendWeights = pfbxSkinDeformer->GetControlPointIndicesCount();
		double *pfControlPointBlendWeights = pfbxSkinDeformer->GetControlPointBlendWeights();
		for (int i = 0; i < nBlendWeights; i++)
		{
			pfbxv4Vertices[i] = pfbxv4DQVertices[i] * pfControlPointBlendWeights[i] + pfbxv4LinearVertices[i] * (1 - pfControlPointBlendWeights[i]);
		}

		delete[] pfbxv4LinearVertices;
		delete[] pfbxv4DQVertices;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
void AnimateFbxMesh(FbxMesh *pfbxMesh, FbxTime& fbxCurrentTime)
{
	int nVertices = pfbxMesh->GetControlPointsCount();
	if (nVertices > 0)
	{
		FbxVector4 *pfbxv4Vertices = new FbxVector4[nVertices];
		::memcpy(pfbxv4Vertices, pfbxMesh->GetControlPoints(), nVertices * sizeof(FbxVector4));

		int nSkinDeformers = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);
		if (nSkinDeformers > 0)
		{
			int nClusters = 0;
			for (int i = 0; i < nSkinDeformers; ++i) nClusters += ((FbxSkin *)(pfbxMesh->GetDeformer(i, FbxDeformer::eSkin)))->GetClusterCount();
			if (nClusters > 0) ::ComputeSkinDeformation(pfbxMesh, fbxCurrentTime, pfbxv4Vertices, nVertices);
		}

		CMeshFromFbx *pMesh = (CMeshFromFbx *)pfbxMesh->GetUserDataPtr();
		if (pMesh)
		{
			for (int i = 0; i < nVertices; i++) pMesh->m_pxmf4MappedPositions[i] = XMFLOAT4((float)pfbxv4Vertices[i][0], (float)pfbxv4Vertices[i][1], (float)pfbxv4Vertices[i][2], 1.0f);
		}

		delete[] pfbxv4Vertices;
	}
}

void AnimateFbxNodeHierarchy(FbxNode *pfbxNode, FbxTime& fbxCurrentTime)
{
	FbxNodeAttribute *pfbxNodeAttribute = pfbxNode->GetNodeAttribute();
	if (pfbxNodeAttribute && (pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		FbxMesh *pfbxMesh = pfbxNode->GetMesh();
		AnimateFbxMesh(pfbxMesh, fbxCurrentTime);
	}

	int nChilds = pfbxNode->GetChildCount();
	for (int i = 0; i < nChilds; i++) AnimateFbxNodeHierarchy(pfbxNode->GetChild(i), fbxCurrentTime);
}

void RenderFbxMesh(ID3D12GraphicsCommandList *pd3dCommandList, FbxMesh *pfbxMesh, FbxAMatrix& fbxmtxNodeToRoot, FbxAMatrix& fbxmtxGeometryOffset, FbxAMatrix fbxmtxWorld)
{
	int nVertices = pfbxMesh->GetControlPointsCount();
	if (nVertices > 0)
	{
		FbxAMatrix fbxmtxTransform = fbxmtxWorld;
		int nSkinDeformers = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);
		if (nSkinDeformers == 0) fbxmtxTransform = fbxmtxWorld * fbxmtxNodeToRoot * fbxmtxGeometryOffset;

		CGameObject::UpdateShaderVariable(pd3dCommandList, &fbxmtxTransform);

		CMeshFromFbx *pMesh = (CMeshFromFbx *)pfbxMesh->GetUserDataPtr();
		if (pMesh) pMesh->Render(pd3dCommandList);
	}
}

void RenderFbxNodeHierarchy(ID3D12GraphicsCommandList *pd3dCommandList, FbxNode *pfbxNode, FbxTime& fbxCurrentTime, FbxAMatrix& fbxmtxWorld)
{
	FbxNodeAttribute *pfbxNodeAttribute = pfbxNode->GetNodeAttribute();
	if (pfbxNodeAttribute && (pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		FbxAMatrix fbxmtxNodeToRoot = pfbxNode->EvaluateGlobalTransform(fbxCurrentTime);
		FbxAMatrix fbxmtxGeometryOffset = GetGeometricTransform(pfbxNode);

		FbxMesh *pfbxMesh = pfbxNode->GetMesh();
		RenderFbxMesh(pd3dCommandList, pfbxMesh, fbxmtxNodeToRoot, fbxmtxGeometryOffset, fbxmtxWorld);
	}

	int nChilds = pfbxNode->GetChildCount();
	for (int i = 0; i < nChilds; i++) RenderFbxNodeHierarchy(pd3dCommandList, pfbxNode->GetChild(i), fbxCurrentTime, fbxmtxWorld);
}

void CreateMeshFromFbxNodeHierarchy(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, FbxNode *pfbxNode)
{
	FbxNodeAttribute *pfbxNodeAttribute = pfbxNode->GetNodeAttribute();
	if (pfbxNodeAttribute && (pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		FbxMesh *pfbxMesh = pfbxNode->GetMesh();
		if (pfbxMesh)
		{
			int nVertices = pfbxMesh->GetControlPointsCount();

			int nIndices = 0;
			int nPolygons = pfbxMesh->GetPolygonCount();
			for (int i = 0; i < nPolygons; i++) nIndices += pfbxMesh->GetPolygonSize(i); //Triangle: 3, Triangulate(), nIndices = nPolygons * 3

			int *pnIndices = new int[nIndices];
			for (int i = 0, k = 0; i < nPolygons; i++)
			{
				int nPolygonSize = pfbxMesh->GetPolygonSize(i); 
				for (int j = 0; j < nPolygonSize; j++) pnIndices[k++] = pfbxMesh->GetPolygonVertex(i, j);
			}

			CMeshFromFbx *pMesh = new CMeshFromFbx(pd3dDevice, pd3dCommandList, nVertices, nIndices, pnIndices);
			pfbxMesh->SetUserDataPtr(pMesh);

			if(pnIndices) delete[] pnIndices;
		}
	}

	int nChilds = pfbxNode->GetChildCount();
	for (int i = 0; i < nChilds; i++) CreateMeshFromFbxNodeHierarchy(pd3dDevice, pd3dCommandList, pfbxNode->GetChild(i));
}

void ReleaseMeshFromFbxNodeHierarchy(FbxNode *pfbxNode)
{
	FbxNodeAttribute *pfbxNodeAttribute = pfbxNode->GetNodeAttribute();
	if (pfbxNodeAttribute && (pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		FbxMesh *pfbxMesh = pfbxNode->GetMesh();
		if (pfbxMesh) 
		{
			CMeshFromFbx *pMesh = (CMeshFromFbx *)pfbxMesh->GetUserDataPtr();
			if (pMesh) pMesh->Release();
		}
	}

	int nChilds = pfbxNode->GetChildCount();
	for (int i = 0; i < nChilds; i++) ReleaseMeshFromFbxNodeHierarchy(pfbxNode->GetChild(i));
}

void ReleaseUploadBufferFromFbxNodeHierarchy(FbxNode *pfbxNode)
{
	FbxNodeAttribute *pfbxNodeAttribute = pfbxNode->GetNodeAttribute();
	if (pfbxNodeAttribute && (pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh))
	{
		FbxMesh *pfbxMesh = pfbxNode->GetMesh();
		if (pfbxMesh) 
		{
			CMeshFromFbx *pMesh = (CMeshFromFbx *)pfbxMesh->GetUserDataPtr();
			if (pMesh) pMesh->ReleaseUploadBuffers();
		}
	}

	int nChilds = pfbxNode->GetChildCount();
	for (int i = 0; i < nChilds; i++) ReleaseUploadBufferFromFbxNodeHierarchy(pfbxNode->GetChild(i));
}

FbxScene *LoadFbxSceneFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, FbxManager *pfbxSdkManager, char *pstrFbxFileName)
{
	FbxScene *pfbxScene = NULL;

	int nSDKMajor, nSDKMinor, nSDKRevision;
	FbxManager::GetFileFormatVersion(nSDKMajor, nSDKMinor, nSDKRevision);

	FbxIOSettings *pfbxIOSettings = pfbxSdkManager->GetIOSettings();
	FbxImporter *pfbxImporter = FbxImporter::Create(pfbxSdkManager, " ");
	bool bImportStatus = pfbxImporter->Initialize(pstrFbxFileName, -1, pfbxIOSettings);

	int nFileFormatMajor, nFileFormatMinor, nFileFormatRevision;
	pfbxImporter->GetFileVersion(nFileFormatMajor, nFileFormatMinor, nFileFormatRevision);

	pfbxScene = FbxScene::Create(pfbxSdkManager, " ");
	bool bStatus = pfbxImporter->Import(pfbxScene);

	FbxGeometryConverter fbxGeomConverter(pfbxSdkManager);
	fbxGeomConverter.Triangulate(pfbxScene, true);
//	fbxGeomConverter.SplitMeshesPerMaterial(pfbxScene, true);

	FbxAxisSystem fbxSceneAxisSystem = pfbxScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem fbxDirectXAxisSystem(FbxAxisSystem::eDirectX);
	if (fbxSceneAxisSystem != fbxDirectXAxisSystem) fbxDirectXAxisSystem.ConvertScene(pfbxScene);
	fbxSceneAxisSystem = pfbxScene->GetGlobalSettings().GetAxisSystem();

	FbxSystemUnit fbxSceneSystemUnit = pfbxScene->GetGlobalSettings().GetSystemUnit();
	if (fbxSceneSystemUnit.GetScaleFactor() != 1.0) FbxSystemUnit::cm.ConvertScene(pfbxScene);

	pfbxImporter->Destroy();

	return(pfbxScene);
}
