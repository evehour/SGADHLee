#pragma once
#include "stdafx.h"

namespace Fbx
{
	struct FbxMaterial
	{
		string Name;

		D3DXCOLOR Diffuse;

		string DiffuseFile;
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

		ModelVertexType Vertex;
	};

	struct FbxMeshPartData
	{
		string MaterialName;

		vector<ModelVertexType> Vertices;
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
		D3DXVECTOR3 Scale;
		D3DXQUATERNION Rotation;
		D3DXVECTOR3 Translation;
	};

	struct FbxKeyframe
	{
		string BoneName;
		vector<FbxKeyframeData> Transforms;
	};

	struct FbxAnimation
	{
		string Name;

		int TotalFrame;
		float FrameRate;
		float Duration; // �ִϸ��̼� ���� �ð�

		vector<FbxKeyframe *> Keyframes;
	};

	struct FbxBoneWeights
	{
	private:
		typedef pair<int, float> Pair; // int: bone idx, float: ������ ���� ��
		vector<Pair> boneWeights; // �ִ� 4��

	public:
		void AddWeights(UINT boneIndex, float boneWeights)
		{

		}
	};
}
