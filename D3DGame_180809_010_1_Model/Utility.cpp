#include "stdafx.h"
#include "Utility.h"

D3DXMATRIX Fbx::Utility::Negative()
{
	D3DXMATRIX scaleX;
	D3DXMatrixScaling(&scaleX, -1, 1, 1);

	
	D3DXMATRIX rotation;
	D3DXMatrixRotationY(&rotation, Math::ToRadian(180));

	return scaleX * rotation;
};

D3DXVECTOR2 Fbx::Utility::ToVector2(FbxVector2 & vec)
{
	return D3DXVECTOR2((float)vec.mData[0], (float)vec.mData[1]);
}

D3DXVECTOR3 Fbx::Utility::ToVector3(FbxVector4 & vec)
{
	return D3DXVECTOR3((float)vec.mData[0], (float)vec.mData[1], (float)vec.mData[2]);
}

D3DXCOLOR Fbx::Utility::ToColor(FbxVector4 & vec)
{
	return D3DXCOLOR((float)vec.mData[0], (float)vec.mData[1], (float)vec.mData[2], 1);
}

D3DXCOLOR Fbx::Utility::ToColor(FbxPropertyT<FbxDouble3>& vec, FbxPropertyT<FbxDouble>& factor)
{
	FbxDouble3 color = vec;

	D3DXCOLOR result;
	result.r = (float)color.mData[0];
	result.g = (float)color.mData[1];
	result.b = (float)color.mData[2];
	result.a = (float)factor;

	return result;
}

D3DXMATRIX Fbx::Utility::ToMatrix(FbxAMatrix & matrix)
{
	FbxVector4 r1 = matrix.GetRow(0);
	FbxVector4 r2 = matrix.GetRow(1);
	FbxVector4 r3 = matrix.GetRow(2);
	FbxVector4 r4 = matrix.GetRow(3);

	D3DXMATRIX origin = D3DXMATRIX
	(
		(float)r1.mData[0], (float)r1.mData[1], (float)r1.mData[2], (float)r1.mData[3],
		(float)r2.mData[0], (float)r2.mData[1], (float)r2.mData[2], (float)r2.mData[3],
		(float)r3.mData[0], (float)r3.mData[1], (float)r3.mData[2], (float)r3.mData[3],
		(float)r4.mData[0], (float)r4.mData[1], (float)r4.mData[2], (float)r4.mData[3]
	);

	return Negative() * origin * Negative();
}

string Fbx::Utility::GetTextureFile(FbxProperty & prop)
{
	if (prop.IsValid())
	{
		if (prop.GetSrcObjectCount() > 0)
		{
			FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>();
			if (texture != NULL)
				return string(texture->GetFileName());
		}
	}
	return "";
}

string Fbx::Utility::GetMaterialName(FbxMesh * mesh, int polygonIndex, int cpIndex)
{
	FbxNode* node = mesh->GetNode();
	if (node == NULL) return "";

	// FbxLayerElementMaterial은 FbxSurfaceMaterial을 상속해서 만들어져 있음.
	// 왜 Layer를 0으로 고정시켜두냐면, Mesh에는 여러개의 레이어가 존재하는데 게임 업계에서는 레이어 중에서 가장 바닥에 있는 0번 인덱스만 사용하기 때문.
	// 만약 레이어를 1번도 쓴다면 가서 싸대기를 날려주고 싶다.
	FbxLayerElementMaterial* material = mesh->GetLayer(0)->GetMaterials();
	if (material == NULL) return "";

	FbxLayerElement::EMappingMode mappingMode = material->GetMappingMode(); // 데이터가 어떤식으로 매핑되어있냐를 가져옴
	FbxLayerElement::EReferenceMode refMode = material->GetReferenceMode(); // 값 참조를 어떤식으로 하게되어있냐를 가져옴

	int mappingIndex = -1;
	switch (mappingMode)
	{
		case FbxLayerElement::eAllSame: mappingIndex = 0; break; // Scene의 Material과 Mesh의 번호가 1:1로 일치되어 매칭될 경우. 이 경우는 Meterial이 하나밖에 없거나 디자이너가 원빈급으로 잘생겨서 정리정돈을 잘 했을 경우에 해당됨. 근데 그럴리가 일반적으로 없음... 후세드
		case FbxLayerElement::eByPolygon: mappingIndex = polygonIndex; break; // 삼각형 기준으로 디자인을 한 것.
		case FbxLayerElement::eByControlPoint: mappingIndex = cpIndex; break; // FBX가 파일로 저장할 때 알아서 처리할 경우가 있음.
		case FbxLayerElement::eByPolygonVertex: mappingIndex = polygonIndex * 3; break; // 정점 기준으로 디자인을 한 것. 원래는 각 정점별 번호를 넣어야 하기 때문에 버텍스번호를 넣어야하지만 그럴 경우에는 멘탈이 박살나므로 걍 삼각형단위로 계산하여 넣음.
		default: assert(false); break; // 다른 경우에는 걍 터트릴꼬얌
	}

	FbxSurfaceMaterial* findMaterial = NULL;
	if (refMode == FbxLayerElement::eDirect) // 바로 참조가능한 경우. 타겟 매핑 인덱스가 곧 Material의 인덱스일 경우.
	{
		if (mappingIndex < node->GetMaterialCount())
			findMaterial = node->GetMaterial(mappingIndex);
	}
	else if (refMode == FbxLayerElement::eIndexToDirect) // 중간에 참조하는 인덱스리스트가 존재하여, 그것을 참조해야만 할 경우.
	{
		FbxLayerElementArrayTemplate<int>& indexArr = material->GetIndexArray(); // 해당 인덱스 리스트를 불러옴.

		if (mappingIndex < indexArr.GetCount())
		{
			int tempIndex = indexArr.GetAt(mappingIndex);

			if (tempIndex < node->GetMaterialCount())
				findMaterial = node->GetMaterial(tempIndex);
		}//if(mappingIndex)
	}//if(refMode)

	if (findMaterial == NULL)
		return  "";
	
	return findMaterial->GetName();
}

D3DXVECTOR2 Fbx::Utility::GetUv(FbxMesh * mesh, int cpIndex, int uvIndex)
{
	D3DXVECTOR2 result = D3DXVECTOR2(0, 0);

	FbxLayerElementUV* uv = mesh->GetLayer(0)->GetUVs();
	if (uv == NULL) return result;

	FbxLayerElement::EMappingMode mappingMode = uv->GetMappingMode();
	FbxLayerElement::EReferenceMode refMode = uv->GetReferenceMode();

	switch (mappingMode)
	{
		case FbxLayerElement::eByControlPoint:
		{
			if (refMode == FbxLayerElement::eDirect)
			{
				result.x = (float)uv->GetDirectArray().GetAt(cpIndex).mData[0];
				result.y = (float)uv->GetDirectArray().GetAt(cpIndex).mData[1];
			}
			else if (refMode == FbxLayerElement::eIndexToDirect)
			{
				int index = uv->GetIndexArray().GetAt(cpIndex);
				result.x = (float)uv->GetDirectArray().GetAt(index).mData[0];
				result.y = (float)uv->GetDirectArray().GetAt(index).mData[1];
			}
		}
		break;

		case FbxLayerElement::eByPolygonVertex:
		{
			result.x = (float)uv->GetDirectArray().GetAt(uvIndex).mData[0];
			result.y = (float)uv->GetDirectArray().GetAt(uvIndex).mData[1];
		}
			break;
	}
	result.y = 1.0f - result.y; //FBX는 UV의 v의 값이 DirectX에서 쓰는 좌표계의 반대방향으로 증가하기 때문에 상하반전을 해줘야 한다.

	return result;
}
