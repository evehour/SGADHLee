#pragma once
#include "stdafx.h"

namespace Fbx
{
	struct FbxMaterial
	{
		string Name;

		string DiffuseFile;
		D3DXCOLOR Diffuse;

		string NormalFile;

		string SpecularFile;
		D3DXCOLOR Specular;
		float SpecularExp;

	};

	struct FbxBoneData
	{
		int Index;
		string Name;

		int Parent;

		D3DXMATRIX LocalTransform;
		D3DXMATRIX GlobalTransform;
	};

	struct FbxVertex
	{
		int ControlPoint;
		string MaterialName;

		VertexTextureNormalTangentBlend Vertex;
	};

	struct FbxMeshPartData
	{
		string MaterialName;

		vector<VertexTextureNormalTangentBlend> Vertices;
		vector<UINT> Indices;
	};

	struct FbxMeshData
	{
		string Name;
		int ParentBone;

		FbxMesh* Mesh;

		vector<FbxVertex *> Vertices;
		vector<FbxMeshPartData *> MeshParts;
	};

	struct FbxKeyframeData
	{
		float Time;

		D3DXVECTOR3 Scale;
		D3DXQUATERNION Rotation;
		D3DXVECTOR3 Translation;
	};

	struct FbxKeyframe
	{
		string BoneName;
		vector<FbxKeyframeData> Transforms;
	};

	struct FbxClip
	{
		string Name;

		int FrameCount;
		float FrameRate;
		float Duration; // 애니메이션 길이 시간

		vector<FbxKeyframe *> Keyframes;
	};

	struct FbxBlendWeight
	{
		D3DXVECTOR4 Indices = D3DXVECTOR4(0, 0, 0, 0);
		D3DXVECTOR4 Weights = D3DXVECTOR4(0, 0, 0, 0);

		void Set(UINT index, UINT boneIndex, float weight)
		{
			float i = (float)boneIndex;
			float w = weight;

			switch (index)
			{
			case 0: Indices.x = i; Weights.x = w; break;
			case 1: Indices.y = i; Weights.y = w; break;
			case 2: Indices.z = i; Weights.z = w; break;
			case 3: Indices.w = i; Weights.w = w; break;
			}
		}
	};

	struct FbxBoneWeights
	{
	private:
		typedef pair<int, float> Pair; // int: bone idx, float: 영향을 받을 값
		vector<Pair> BoneWeights; // 최대 4개

	public:
		void AddWeights(UINT boneIndex, float boneWeights)
		{
			if (boneWeights <= 0.0f) return;
			
			bool bAdd = false;
			vector<Pair>::iterator it = BoneWeights.begin();
			//작은것부터 순서대로 넣기 위해 아래짓을 함.
			while (it != BoneWeights.end())
			{
				if (boneWeights > it->second)
				{
					BoneWeights.insert(it, Pair(boneIndex, boneWeights));
					bAdd = true;

					break;
				}

				it++;
			}//while(it)

			if (!bAdd)
				BoneWeights.push_back(Pair(boneIndex, boneWeights));
		}

		void GetBlendWeights(FbxBlendWeight& blendWeights)
		{
			for (UINT i = 0; i < BoneWeights.size(); i++)
			{
				if (i >= 4) return;

				// 4개중에 하나 찾아서 세팅. 앞에가 번호, 앞에가 가중치.
				blendWeights.Set(i, BoneWeights[i].first, BoneWeights[i].second);
			}
		}

		void Normalize()
		{
			float totalWeight = 0.0f;

			int i = 0;
			vector<Pair>::iterator it = BoneWeights.begin();
			while (it != BoneWeights.end())
			{
				if (i < 4)
				{
					totalWeight += it->second;
					i++; it++;
				}
				else
					it = BoneWeights.erase(it);
			}

			float scale = 1.0f / totalWeight;

			it = BoneWeights.begin();
			while (it != BoneWeights.end())
			{
				it->second *= scale;
				it++;
			}
		}
	};
}
