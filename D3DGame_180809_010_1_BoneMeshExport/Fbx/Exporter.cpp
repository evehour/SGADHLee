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
	ios->SetBoolProp(IMP_FBX_TEXTURE, true); // 만약 FBX안에 텍스쳐파일이 있다면 FBX파일이 있는 곳에 추출하라. 인데 기본옵션으로 이미 true가 되어있음.
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
	//TODO: 이 주석은 보기 -> 작업 목록에서 볼 수 있다. 현업에서 많이 쓰인다고 함. 개꿀?!
	//TODO: 현재 diffuse만 추출하고 있다. nomalmap과 specular도 추출해야한다.
	WriteMaterial(saveFolder, fileName);
}

void Fbx::Exporter::ExportMesh(wstring saveFolder, wstring fileName)
{
	ReadBoneData(scene->GetRootNode(), -1, -1); // root는 index를 첫시작이므로 -1로 줌(root의 실제 index는 0), 부모가 없으므로 -1
	ReadSkinData();
	WriteMeshData(saveFolder, fileName);
}

void Fbx::Exporter::ReadMaterial()
{
	int count = scene->GetMaterialCount(); // Scene의 전체 Material의 수를 조회.

	for (int i = 0; i < count; i++)
	{
		FbxSurfaceMaterial* fbxMaterial = scene->GetMaterial(i);

		FbxMaterial* material = new FbxMaterial();
		material->Name = fbxMaterial->GetName();

		if (fbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId) == true) // 다운캐스팅이 가능하냐
		{
			//렘버트 쉐이딩이냐?
			//FbxSurfaceLambert* lambert = (FbxSurfaceLambert*)fbxMaterial;
			FbxSurfaceLambert* lambert = reinterpret_cast<FbxSurfaceLambert*>(fbxMaterial);

			material->Diffuse = Utility::ToColor(lambert->Diffuse, lambert->DiffuseFactor);
		}

		// prop을 통해 텍스쳐 파일 이름을 가져올 수 있음.
		FbxProperty prop;
		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);// sDiffuse는 파일 이름이다. 결국 이름으로 찾는다는 소리임
		material->DiffuseFile = Utility::GetTextureFile(prop); // 현재 Material의 텍스쳐 이름을 가져와서 저장. 이 때 이름은 절대경로이다.
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
		WriteXmlColor(document, element, material->Diffuse);
		node->LinkEndChild(element);

		element = document->NewElement("DiffuseFile");
		CopyTextureFile(material->DiffuseFile, saveFolder);
		element->SetText(material->DiffuseFile.c_str());
		node->LinkEndChild(element);

		SAFE_DELETE(material);
	}
	
	string file = String::ToString(saveFolder + fileName);
	document->SaveFile(file.c_str()); // XML형태 파일로 최종적으로 저장.
}

void Fbx::Exporter::ReadBoneData(FbxNode * node, int index, int parent)
{
	// 이 함수는 Tree구조로 저장되어 있기 때문에 제귀함수로 돌아간다.

	FbxNodeAttribute* attribute = node->GetNodeAttribute(); // 현재 Node의 속성을 조회.
	
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
				// FbxGeometryConverter::Triangulate는 3DMAX는 Mesh의 기본형태가 사각형이다.
				// 우리는 기본형태가 삼각형이므로 맞지 않는다. 기본형태를 사각형에서 삼각형으로 바꿔주는 고마운 쉐끼루이다.
				// 1번째 인자값으로 연결된 노드를 찾음.
				// 2번째 인자값은 scene에 저장할꺼냐를 묻는거. true면 해당 노드의 메쉬데이터가 사각->삼각으로 변경되어 저장됨. false하면 저장이 안되므로 리턴해준걸 받아서 써야함.
				// 3번째 인자값은 신형방식으로 쓸거냐를 묻는거. true로 하면 구형방식을 쓴다는 것으로써 좀 더 단순함.
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
		assert(vertexInPolygon == 3); // 삼각형의 정점이 꼭 3개인게 아니다(!!!!). 그래서 3개가 아니면 터트리게 했다. 터지면 제작님하한테 가서 귀싸대기 날리자.

		for (int vi = vertexInPolygon - 1; vi >= 0; vi--)
		{
			FbxVertex* vertex = new FbxVertex();

			// Control Point Index. 폴리곤(삼각형)의 정점 인덱스를 반환.
			// 그럼 그냥 vi로 하면 되지, 뭐하러 cpIndex를 만드냐면 FBX안에는 정점의 정보들이 가지런히, 이뿌게, 정렬되어있지 않아서 그렇다고 함. ㅅㅂ...
			int cpIndex = mesh->GetPolygonVertex(p, vi);
			vertex->ControlPoint = cpIndex;



			D3DXVECTOR3 temp;

			FbxVector4 position = mesh->GetControlPointAt(cpIndex); // 해당 정점 인덱스의 위치정보를 반환.
			temp = Utility::ToVector3(position); // 우리가 사용하는 D3DXVECTOR3 형식으로 반환.
			D3DXVec3TransformCoord(&vertex->Vertex.Position, &temp, &Utility::Negative()); // FBX는 오른손 좌표계로써 정렬되어있다. 이걸 왼손 좌표계로 바꾸기 위해 (좌우반전 + Y축으로 180도(PI) 회전)이 필요하다.

			FbxVector4 normal;
			mesh->GetPolygonVertexNormal(p, vi, normal); // normal에는 레퍼런스인자값으로 주기 때문에 값이 저장됨
			normal.Normalize(); // noraml을 왜 또 노멀라이즈 해주냐면 nomal에는 크기값이 들어가있다. 하지만 우리가 쓸 때에는 크기값이 있으면 쓰기 불편하므로 방향만을 두기 위해 0~1값인 Nomalize화 한 값을 저장한다.
			temp = Utility::ToVector3(normal);
			D3DXVec3TransformCoord(&vertex->Vertex.Normal, &temp, &Utility::Negative()); // 이건 위와 마찬가지...



			vertex->MaterialName = Utility::GetMaterialName(mesh, p, cpIndex);

			int uvIndex = mesh->GetTextureUVIndex(p, vi); // 해당 정점의 UV의 index를 조회.
			vertex->Vertex.Uv = Utility::GetUv(mesh, cpIndex, uvIndex);

			vertices.push_back(vertex);
		}
	}//for(p)

	FbxMeshData* meshData = new FbxMeshData();
	meshData->Name = node->GetName();
	meshData->ParentBone = parentBone; // 해당 Mesh가 덮어지는 Bone
	meshData->Vertices = vertices; // Mesh의 정점 정보
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


			FbxMeshPartData* meshPart = new FbxMeshPartData(); // meshPart가 실제 그려질 정보를 갖고있음.
			meshPart->MaterialName = materialName;

			for (FbxVertex* temp : gather)
			{
				ModelVertexType vertex;
				vertex = temp->Vertex;

				meshPart->Vertices.push_back(vertex);
				meshPart->Indices.push_back(meshPart->Indices.size()); // 원래는 계산해야하지만 그냥 순차적으로 넣는다고 하셨다.
			}
			meshData->MeshParts.push_back(meshPart);
		}
	}
}

void Fbx::Exporter::WriteMeshData(wstring saveFolder, wstring fileName)
{
	//파일을 쓰는 순서에 맞춰서 읽어야 한다... 는 존나 해봤고 존나 엿먹어봤으니 잘 할것이다. 보험데이터 ㅅㅂ
	//가변데이터를 최대한 없애기 위해 char[]을 썼는데 우리는 string을 썼기 때문에 죠지워싱턴이 된 것이다. 
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

		SAFE_DELETE(bone); // 더 이상 데이터가 필요없으므로 메모리를 반환.
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
			w->Byte(&part->Vertices[0], sizeof(ModelVertexType) * part->Vertices.size()); // 가변 데이터가 없으므로 struct째로 넣음.

			w->UInt(part->Indices.size());
			w->Byte(&part->Indices[0], sizeof(UINT) * part->Indices.size()); // 가변 데이터가 없으므로 struct째로 넣음.

			SAFE_DELETE(part);
		}

		SAFE_DELETE(meshData); // 더 이상 데이터가 필요없으므로 메모리를 반환.
	}

	w->Close(); // 파일을 저장 후 스트림 닫기.
	SAFE_DELETE(w); // 메모리 반환
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
	wstring fileName = Path::GetFileName(file); // 절대경로에서 파일이름과 확장자만 추출.

	if (Path::ExistFile(textureFile))
		CopyFile(file.c_str(), (saveFolder + fileName).c_str(), FALSE); // 텍스쳐파일이 있다면 대상 저장폴더에 복사.

	textureFile = String::ToString(fileName); // 절대경로에서 파일이름과 확장자만 추출한 문자열을 저장
}
