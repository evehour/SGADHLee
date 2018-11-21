#pragma once

namespace Xml
{
	class XMLDocument;
	class XMLElement;
}

class ModelBone;
class ModelMesh;

class Model
{
public:
	Model();
	~Model();

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

	ModelBone* AddBone();

	void ReadMaterial(wstring folder, wstring file);
	void ReadMesh(wstring folder, wstring file);

	void WriteModel(wstring file);
	void WriteMesh(wstring folder, wstring file);
	void WriteMaterial(wstring folder, wstring file);

	void CopyGlobalBoneTo(D3DXMATRIX* transforms);
	void CopyGlobalBoneTo(D3DXMATRIX* transforms, D3DXMATRIX& w);

private:
	void BindingBone();
	void BindingMesh();

	void WriteXmlColor(Xml::XMLDocument * document, Xml::XMLElement * element, D3DXCOLOR & color);

private:
	wstring materialFileName;
	wstring meshFileName;

	class ModelBone* root;

	vector<Material *> materials;
	vector<class ModelMesh *> meshes;
	vector<class ModelBone *> bones;
};

class Models
{
public:
	friend class Model;

public:
	static void Create();
	static void Delete();

private:
	static void LoadMaterial(wstring file, vector<Material *>* materials);
	static void ReadMaterialData(wstring file);

	static void LoadMesh(wstring file, vector<class ModelBone *>* bones, vector<class ModelMesh *>* meshes);
	static void ReadMeshData(wstring file);

private:
	static map<wstring, vector<Material *>> materialMap;

	struct MeshData
	{
		vector<class ModelBone *> Bones;
		vector<class ModelMesh *> Meshes;
	};
	static map<wstring, MeshData> meshDataMap;
};