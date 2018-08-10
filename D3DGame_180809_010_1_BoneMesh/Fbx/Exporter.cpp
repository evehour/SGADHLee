#include "stdafx.h"
#include "Exporter.h"
#include "Type.h"
#include "Utility.h"
#include "./Utilities/Xml.h"
#include "./Utilities/BinaryFile.h"

Fbx::Exporter::Exporter(wstring file)
{
	manager = FbxManager::Create();
	scene = FbxScene::Create(manager, "");

	ios = FbxIOSettings::Create(manager, IOSROOT);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true); // ���� FBX�ȿ� �ؽ��������� �ִٸ� FBX������ �ִ� ���� �����϶�. �ε� �⺻�ɼ����� �̹� true�� �Ǿ�����.
	manager->SetIOSettings(ios);

	importer = FbxImporter::Create(manager, "");

	// ������ ���� ��
	string sFile = String::ToString(file);
	bool b = importer->Initialize(sFile.c_str(), -1, ios);
	assert(b);
	
	// ���� ������ scnee�� �Է�
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
	//TODO: �� �ּ��� ���� -> �۾� ��Ͽ��� �� �� �ִ�. �������� ���� ���δٰ� ��. ����?!
	//TODO: ���� diffuse�� �����ϰ� �ִ�. nomalmap�� specular�� �����ؾ��Ѵ�.
	WriteMaterial(saveFolder, fileName);
}

void Fbx::Exporter::ExportMesh(wstring saveFolder, wstring fileName)
{
	ReadBoneData(scene->GetRootNode(), -1, -1); // root�� index�� ù�����̹Ƿ� -1�� ��(root�� ���� index�� 0), �θ� �����Ƿ� -1
	ReadSkinData();
	WriteMeshData(saveFolder, fileName);
}

void Fbx::Exporter::ReadMaterial()
{
	int count = scene->GetMaterialCount(); // Scene�� ��ü Material�� ���� ��ȸ.

	for (int i = 0; i < count; i++)
	{
		FbxSurfaceMaterial* fbxMaterial = scene->GetMaterial(i);

		FbxMaterial* material = new FbxMaterial();
		material->Name = fbxMaterial->GetName();

		if (fbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId) == true) // �ٿ�ĳ������ �����ϳ�
		{
			//����Ʈ ���̵��̳�?
			//FbxSurfaceLambert* lambert = (FbxSurfaceLambert*)fbxMaterial;
			FbxSurfaceLambert* lambert = reinterpret_cast<FbxSurfaceLambert*>(fbxMaterial);

			material->Diffuse = Utility::ToColor(lambert->Diffuse, lambert->DiffuseFactor);
		}

		// prop�� ���� �ؽ��� ���� �̸��� ������ �� ����.
		FbxProperty prop;
		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);// sDiffuse�� ���� �̸��̴�. �ᱹ �̸����� ã�´ٴ� �Ҹ���
		material->DiffuseFile = Utility::GetTextureFile(prop); // ���� Material�� �ؽ��� �̸��� �����ͼ� ����. �� �� �̸��� �������̴�.
		materials.push_back(material);
	}
}

void Fbx::Exporter::WriteMaterial(wstring saveFolder, wstring fileName)
{
	// �������� ������ ������ �ڵ����� �������.
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
		WriteXmlColor(document, element, material->Diffuse);
		node->LinkEndChild(element);

		element = document->NewElement("DiffuseFile");
		CopyTextureFile(material->DiffuseFile, saveFolder);
		element->SetText(material->DiffuseFile.c_str());
		node->LinkEndChild(element);

		SAFE_DELETE(material);
	}
	
	string file = String::ToString(saveFolder + fileName);
	document->SaveFile(file.c_str()); // XML���� ���Ϸ� ���������� ����.
}

void Fbx::Exporter::ReadBoneData(FbxNode * node, int index, int parent)
{
	// �� �Լ��� Tree������ ����Ǿ� �ֱ� ������ �����Լ��� ���ư���.

	FbxNodeAttribute* attribute = node->GetNodeAttribute(); // ���� Node�� �Ӽ��� ��ȸ.
	
	if (attribute)
	{
		FbxNodeAttribute::EType nodeType = attribute->GetAttributeType();

		bool b = false;
		b |= (nodeType == FbxNodeAttribute::eSkeleton);
		b |= (nodeType == FbxNodeAttribute::eMesh);
		b |= (nodeType == FbxNodeAttribute::eNull);
		b |= (nodeType == FbxNodeAttribute::eMarker);

		if (b)
		{
			FbxBoneData* bone = new FbxBoneData();
			bone->Index = index;
			bone->Parent = parent;
			bone->Name = node->GetName();
			bone->LocalTransform = Utility::ToMatrix(node->EvaluateLocalTransform());
			bone->GlobalTransform = Utility::ToMatrix(node->EvaluateGlobalTransform());
			boneDatas.push_back(bone);;

			if (nodeType == FbxNodeAttribute::eMesh)
			{
				// FbxGeometryConverter::Triangulate�� 3DMAX�� Mesh�� �⺻���°� �簢���̴�.
				// �츮�� �⺻���°� �ﰢ���̹Ƿ� ���� �ʴ´�. �⺻���¸� �簢������ �ﰢ������ �ٲ��ִ� ���� �������̴�.
				// 1��° ���ڰ����� ����� ��带 ã��.
				// 2��° ���ڰ��� scene�� �����Ҳ��ĸ� ���°�. true�� �ش� ����� �޽������Ͱ� �簢->�ﰢ���� ����Ǿ� �����. false�ϸ� ������ �ȵǹǷ� �������ذ� �޾Ƽ� �����.
				// 3��° ���ڰ��� ����������� ���ųĸ� ���°�. true�� �ϸ� ��������� ���ٴ� �����ν� �� �� �ܼ���.
				converter->Triangulate(attribute, true, true);

				ReadMeshData(node, index);
			}
		}//if(b)
	}
	
	for (int i = 0; i < node->GetChildCount(); i++)
	{
		ReadBoneData(node->GetChild(i), boneDatas.size(), index);
	}
}

void Fbx::Exporter::ReadMeshData(FbxNode * node, int parentBone)
{
	FbxMesh* mesh = node->GetMesh();

	vector<FbxVertex *> vertices;
	for (int p = 0; p < mesh->GetPolygonCount(); p++)
	{
		int vertexInPolygon = mesh->GetPolygonSize(p);
		assert(vertexInPolygon == 3); // �ﰢ���� ������ �� 3���ΰ� �ƴϴ�(!!!!). �׷��� 3���� �ƴϸ� ��Ʈ���� �ߴ�. ������ ���۴������� ���� �ͽδ�� ������.

		for (int vi = vertexInPolygon - 1; vi >= 0; vi--)
		{
			FbxVertex* vertex = new FbxVertex();

			// Control Point Index. ������(�ﰢ��)�� ���� �ε����� ��ȯ.
			// �׷� �׳� vi�� �ϸ� ����, ���Ϸ� cpIndex�� ����ĸ� FBX�ȿ��� ������ �������� ��������, �̻Ѱ�, ���ĵǾ����� �ʾƼ� �׷��ٰ� ��. ����...
			int cpIndex = mesh->GetPolygonVertex(p, vi);
			vertex->ControlPoint = cpIndex;



			D3DXVECTOR3 temp;

			FbxVector4 position = mesh->GetControlPointAt(cpIndex); // �ش� ���� �ε����� ��ġ������ ��ȯ.
			temp = Utility::ToVector3(position); // �츮�� ����ϴ� D3DXVECTOR3 �������� ��ȯ.
			D3DXVec3TransformCoord(&vertex->Vertex.Position, &temp, &Utility::Negative()); // FBX�� ������ ��ǥ��ν� ���ĵǾ��ִ�. �̰� �޼� ��ǥ��� �ٲٱ� ���� (�¿���� + Y������ 180��(PI) ȸ��)�� �ʿ��ϴ�.

			FbxVector4 normal;
			mesh->GetPolygonVertexNormal(p, vi, normal); // normal���� ���۷������ڰ����� �ֱ� ������ ���� �����
			normal.Normalize(); // noraml�� �� �� ��ֶ����� ���ֳĸ� nomal���� ũ�Ⱚ�� ���ִ�. ������ �츮�� �� ������ ũ�Ⱚ�� ������ ���� �����ϹǷ� ���⸸�� �α� ���� 0~1���� Nomalizeȭ �� ���� �����Ѵ�.
			temp = Utility::ToVector3(normal);
			D3DXVec3TransformCoord(&vertex->Vertex.Normal, &temp, &Utility::Negative()); // �̰� ���� ��������...



			vertex->MaterialName = Utility::GetMaterialName(mesh, p, cpIndex);

			int uvIndex = mesh->GetTextureUVIndex(p, vi); // �ش� ������ UV�� index�� ��ȸ.
			vertex->Vertex.Uv = Utility::GetUv(mesh, cpIndex, uvIndex);

			vertices.push_back(vertex);
		}
	}//for(p)

	FbxMeshData* meshData = new FbxMeshData();
	meshData->Name = node->GetName();
	meshData->ParentBone = parentBone; // �ش� Mesh�� �������� Bone
	meshData->Vertices = vertices; // Mesh�� ���� ����
	meshData->Mesh = mesh;
	meshDatas.push_back(meshData);

}

void Fbx::Exporter::ReadSkinData()
{
	for (FbxMeshData* meshData : meshDatas)
	{
		for (int i = 0; i < scene->GetMaterialCount(); i++)
		{
			FbxSurfaceMaterial* material = scene->GetMaterial(i);
			string materialName = material->GetName();

			vector<FbxVertex*> gather;
			for (FbxVertex* temp : meshData->Vertices)
			{
				if (temp->MaterialName == materialName)
					gather.push_back(temp);
			}
			if (gather.size() < 1) continue;


			FbxMeshPartData* meshPart = new FbxMeshPartData(); // meshPart�� ���� �׷��� ������ ��������.
			meshPart->MaterialName = materialName;

			for (FbxVertex* temp : gather)
			{
				ModelVertexType vertex;
				vertex = temp->Vertex;

				meshPart->Vertices.push_back(vertex);
				meshPart->Indices.push_back(meshPart->Indices.size()); // ������ ����ؾ������� �׳� ���������� �ִ´ٰ� �ϼ̴�.
			}
			meshData->MeshParts.push_back(meshPart);
		}
	}
}

void Fbx::Exporter::WriteMeshData(wstring saveFolder, wstring fileName)
{
	//������ ���� ������ ���缭 �о�� �Ѵ�... �� ���� �غð� ���� ���Ծ������ �� �Ұ��̴�. ���赥���� ����
	//���������͸� �ִ��� ���ֱ� ���� char[]�� ��µ� �츮�� string�� ��� ������ ������������ �� ���̴�. 
	Path::CreateFolder(saveFolder);


	BinaryWriter* w = new BinaryWriter();
	w->Open(saveFolder + fileName);

	w->UInt(boneDatas.size());
	for (FbxBoneData* bone : boneDatas)
	{
		w->Int(bone->Index);
		w->String(bone->Name);
		w->Int(bone->Parent);

		w->Matrix(bone->LocalTransform);
		w->Matrix(bone->GlobalTransform);

		SAFE_DELETE(bone); // �� �̻� �����Ͱ� �ʿ�����Ƿ� �޸𸮸� ��ȯ.
	}

	w->UInt(meshDatas.size());
	for (FbxMeshData* meshData : meshDatas)
	{
		w->String(meshData->Name);
		w->Int(meshData->ParentBone);

		w->UInt(meshData->MeshParts.size());
		for (FbxMeshPartData* part : meshData->MeshParts)
		{
			w->String(part->MaterialName);

			w->UInt(part->Vertices.size());
			w->Byte(&part->Vertices[0], sizeof(ModelVertexType) * part->Vertices.size()); // ���� �����Ͱ� �����Ƿ� struct°�� ����.

			w->UInt(part->Indices.size());
			w->Byte(&part->Indices[0], sizeof(UINT) * part->Indices.size()); // ���� �����Ͱ� �����Ƿ� struct°�� ����.

			SAFE_DELETE(part);
		}

		SAFE_DELETE(meshData); // �� �̻� �����Ͱ� �ʿ�����Ƿ� �޸𸮸� ��ȯ.
	}

	w->Close(); // ������ ���� �� ��Ʈ�� �ݱ�.
	SAFE_DELETE(w); // �޸� ��ȯ
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
	wstring fileName = Path::GetFileName(file); // �����ο��� �����̸��� Ȯ���ڸ� ����.

	if (Path::ExistFile(textureFile))
		CopyFile(file.c_str(), (saveFolder + fileName).c_str(), FALSE); // �ؽ��������� �ִٸ� ��� ���������� ����.

	textureFile = String::ToString(fileName); // �����ο��� �����̸��� Ȯ���ڸ� ������ ���ڿ��� ����
}
