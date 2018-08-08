#include "stdafx.h"
#include "Exporter.h"
#include "Type.h"
#include "Utility.h"
#include "./Utilities/Xml.h"

Fbx::Exporter::Exporter(wstring file)
{
	manager = FbxManager::Create();
	scene = FbxScene::Create(manager, "");

	ios = FbxIOSettings::Create(manager, IOSROOT);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);
	manager->SetIOSettings(ios);

	importer = FbxImporter::Create(manager, "");

	// 파일을 여는 것
	string sFile = String::ToString(file);
	bool b = importer->Initialize(sFile.c_str(), -1, ios);
	assert(b);
	
	// 열은 파일을 scnee에 입력
	b = importer->Import(scene);
	assert(b);

	converter = new FbxGeometryConverter(manager);
}

Fbx::Exporter::~Exporter()
{
	SAFE_DELETE(converter);
	ios->Destroy();
	importer->Destroy();
	scene->Destroy();
	manager->Destroy();
}

void Fbx::Exporter::ExportMaterial(wstring saveFolder, wstring fileName)
{
	ReadMaterial();
	WriteMaterial(saveFolder, fileName);
}

void Fbx::Exporter::ReadMaterial()
{
	int count = scene->GetMaterialCount();

	for (int i = 0; i < count; i++)
	{
		FbxSurfaceMaterial* fbxMaterial = scene->GetMaterial(i);

		FbxMaterial* material = new FbxMaterial();
		material->Name = fbxMaterial->GetName();

		if (fbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId) == true) // 다운캐스팅이 가능하냐고 묻는것
		{
			FbxSurfaceLambert* lambert = (FbxSurfaceLambert*)fbxMaterial;

			material->Diffuse = Utility::ToColor(lambert->Diffuse, lambert->DiffuseFactor);
		}

		FbxProperty prop;

		// sDiffuse는 이름이다. 이름으로 찾는다는 소리임
		// prop을 통해 텍스쳐 파일 이름을 가져올 수 있음.
		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
		material->DiffuseFile = Utility::GetTextureFile(prop);
		materials.push_back(material);
	}
}

void Fbx::Exporter::WriteMaterial(wstring saveFolder, wstring fileName)
{
	// 하위까지 폴더가 없으면 자동으로 만들어줌.
	Path::CreateFolders(saveFolder);

	Xml::XMLDocument* document = new Xml::XMLDocument();
	Xml::XMLDeclaration *decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	Xml::XMLElement* root = document->NewElement("Materials");
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);

	for (FbxMaterial* material : materials)
	{
		Xml::XMLElement* node = document->NewElement("Material");
		root->LinkEndChild(node);

		Xml::XMLElement* element = NULL;

		element = document->NewElement("Name");
		element->SetText(material->Name.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Diffuse");
		node->LinkEndChild(element);

		WriteXmlColor(document, element, material->Diffuse);

		element = document->NewElement("DiffuseFile");
		CopyTextureFile(material->DiffuseFile, saveFolder);
		element->SetText(material->DiffuseFile.c_str());
		node->LinkEndChild(element);
	}
	/*
	Xml::XMLElement* temp1 = document->NewElement("Temp1");
	temp1->SetText(10.0f);
	root->LinkEndChild(temp1);

	Xml::XMLElement* temp2 = document->NewElement("Temp2");
	temp2->SetText(20);
	root->LinkEndChild(temp2);
	*/
	string file = String::ToString(saveFolder + fileName);
	document->SaveFile(file.c_str());
}

void Fbx::Exporter::WriteXmlColor(Xml::XMLDocument* document, Xml::XMLElement * element, D3DXCOLOR & color)
{
	Xml::XMLElement* r = document->NewElement("R");
	r->SetText(color.r);
	element->LinkEndChild(r);

	Xml::XMLElement* g = document->NewElement("G");
	g->SetText(color.g);
	element->LinkEndChild(g);

	Xml::XMLElement* b = document->NewElement("B");
	b->SetText(color.b);
	element->LinkEndChild(b);

	Xml::XMLElement* a = document->NewElement("A");
	a->SetText(color.a);
	element->LinkEndChild(a);
}

void Fbx::Exporter::CopyTextureFile(OUT string & textureFile, wstring & saveFolder)
{
	if (textureFile.length() < 1) return;

	wstring file = String::ToWString(textureFile);
	wstring fileName = Path::GetFileName(file);

	if (Path::ExistFile(textureFile))
		CopyFile(file.c_str(), (saveFolder + fileName).c_str(), FALSE);


	textureFile = String::ToString(fileName);
}
