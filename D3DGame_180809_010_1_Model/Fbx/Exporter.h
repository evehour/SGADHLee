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
	public:
		Exporter(wstring file);
		~Exporter();

		void ExportMaterial(wstring saveFolder, wstring fileName);

	private:
		void ReadMaterial();
		void WriteMaterial(wstring saveFolder, wstring fileName);

	private:
		FbxManager * manager;
		FbxImporter* importer;
		FbxScene* scene;
		FbxIOSettings* ios;
		FbxGeometryConverter* converter;

	private:
		void WriteXmlColor(Xml::XMLDocument* document, Xml::XMLElement* element, D3DXCOLOR& color);
		void CopyTextureFile(OUT string& textureFile, wstring& saveFolder);
	private:
		vector<struct FbxMaterial*> materials;
	};
}