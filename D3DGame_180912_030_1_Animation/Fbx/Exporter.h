#pragma once

namespace Xml
{
	class XMLDocument;
	class XMLElement;
}

namespace Fbx
{
	class Exporter
	{
	private:
		D3DXVECTOR3 scale;
	public:
		Exporter(wstring file);
		~Exporter();

		void ExportMaterial(wstring saveFolder, wstring fileName);
		void ExportMesh(wstring saveFolder, wstring fileName, D3DXVECTOR3 scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f));

		void GetClipList(vector<wstring>* list);
		void ExportAnimation(wstring saveFolder, wstring fileName, UINT clipNumber);
		void ExportAnimation(wstring saveFolder, wstring fileName, wstring clipName);

	private:
		void ReadMaterial();
		void WriteMaterial(wstring saveFolder, wstring fileName);

		void ReadBoneData(FbxNode* node, int index, int parent, D3DXVECTOR3 scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f));

		UINT GetBoneIndexByName(string name);
		void ReadMeshData(FbxNode* node, int parentBone);
		void ReadSkinData(D3DXVECTOR3 scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f));
		void WriteMeshData(wstring saveFolder, wstring fileName, D3DXVECTOR3 scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f));

		struct FbxClip* ReadAnimationData(UINT index);
		struct FbxClip* ReadAnimationData(wstring name);
		void ReadKeyframeData(struct FbxClip* clip, FbxNode* node, int start, int stop);

		void WriteClipData(struct FbxClip* clip, wstring saveFolder, wstring fileName);

	private:
		FbxManager* manager;
		FbxImporter* importer;
		FbxScene* scene;
		FbxIOSettings* ios;
		FbxGeometryConverter* converter;

	private:
		void WriteXmlColor
		(
			Xml::XMLDocument* document
			, Xml::XMLElement* element
			, D3DXCOLOR& color
		);

		void CopyTextureFile(OUT string& textureFile, wstring& saveFolder);

	private:
		vector<struct FbxMaterial *> materials;
		
		vector<struct FbxBoneData *> boneDatas;
		vector<struct FbxMeshData *> meshDatas;
	};
}