#pragma once

class ModelBone;
class ModelMesh;

class Model
{
public:
	Model();
	~Model();

	static void Delete();

	UINT MaterialCount() { return materials.size(); }
	vector<Material *>& Materials() { return materials; }
	Material* MaterialByIndex(UINT index) { return materials[index]; }
	Material* MaterialByName(wstring name);

	UINT BoneCount() { return bones.size(); }
	vector<ModelBone *>& Bones() { return bones; }
	ModelBone* BoneByIndex(UINT index) { return bones[index]; }
	ModelBone* BoneByName(wstring name);

	UINT MeshCount() { return meshes.size(); }
	vector<ModelMesh *>& Meshes() { return meshes; }
	ModelMesh* MeshByIndex(UINT index) { return meshes[index]; }
	ModelMesh* MeshByName(wstring name);


	void ReadMaterial(wstring folder, wstring file);
	void LoadMaterial(wstring folder, wstring file);
	void ReadMesh(wstring folder, wstring file);
	void LoadMesh(wstring folder, wstring file);

	void CopyGlobalBoneTo(vector<D3DXMATRIX>& transforms);
	void CopyGlobalBoneTo(vector<D3DXMATRIX>& transforms, D3DXMATRIX& w);

private:
	void BindingBone();
	void BindingMesh();

private:
	class ModelBone* root;

	vector<Material *> materials;
	vector<class ModelMesh *> meshes;
	vector<class ModelBone *> bones;

	struct MeshData
	{
		vector<class ModelMesh *> Meshes;
		vector<class ModelBone *> Bones;
	};

	static map<wstring, vector<Material  *>> materialMap;
	static map<wstring, MeshData> meshDataMap;

private:
	class BoneBuffer : public ShaderBuffer
	{
	public:
		BoneBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			for (int i = 0; i < 128; i++)
				D3DXMatrixIdentity(&Data.Bones[i]);
		}

		void Bones(D3DXMATRIX* m, UINT count)
		{
			memcpy(Data.Bones, m, sizeof(D3DXMATRIX) * count);
			
			for (UINT i = 0; i < count; i++)
				D3DXMatrixTranspose(&Data.Bones[i], &Data.Bones[i]);
		}

	private:
		struct Struct
		{
			D3DXMATRIX Bones[128];
		} Data;
	};

	BoneBuffer* buffer;

public:
	BoneBuffer* Buffer() { return buffer; }
};