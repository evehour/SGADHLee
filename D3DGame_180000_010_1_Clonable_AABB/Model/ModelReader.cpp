#include "stdafx.h"
#include "Model.h"
#include "ModelBone.h"
#include "ModelMesh.h"
#include "ModelMeshPart.h"
#include "../Utilities/Xml.h"
#include "../Utilities/BinaryFile.h"

#define USING_ICLONABLE	false

map<wstring, vector<Material  *>> Model::materialMap;
map<wstring, Model::MeshData> Model::meshDataMap;

void Model::ReadMaterial(wstring folder, wstring file)
{
	wstring tempStr = folder + file;
#if USING_ICLONABLE
	if (materialMap.count(tempStr) < 1)
		LoadMaterial(folder, file);
	else
	{
		for (Material* material : materialMap[tempStr])
		{
			Material* temp = NULL;
			material->Clone((void**)&temp);

			materials.push_back(temp);
		}
	}
#else
	if (materialMap.count(tempStr) < 1)
		LoadMaterial(folder, file);
	else
		materials = materialMap[tempStr];
#endif
}

void Model::LoadMaterial(wstring folder, wstring file)
{
	Xml::XMLDocument* document = new Xml::XMLDocument();

	wstring tempFile = folder + file;
	Xml::XMLError error = document->LoadFile(String::ToString(tempFile).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement* root = document->FirstChildElement();
	Xml::XMLElement* matNode = root->FirstChildElement();

	do
	{
		Xml::XMLElement* node = NULL;

		Material* material = new Material();

		node = node = matNode->FirstChildElement();
		material->Name(String::ToWString(node->GetText()));


		node = node->NextSiblingElement();

		D3DXCOLOR dxColor;
		Xml::XMLElement* color = node->FirstChildElement();
		dxColor.r = color->FloatText();

		color = color->NextSiblingElement();
		dxColor.g = color->FloatText();

		color = color->NextSiblingElement();
		dxColor.b = color->FloatText();

		color = color->NextSiblingElement();
		dxColor.a = color->FloatText();
		material->SetDiffuse(dxColor);


		node = node->NextSiblingElement();
		wstring diffuseTexture = String::ToWString(node->GetText());
		if (diffuseTexture.length() > 0)
			material->SetDiffuseMap(folder + diffuseTexture);


		materials.push_back(material);
		matNode = matNode->NextSiblingElement();
	} while (matNode != NULL);

	materialMap[tempFile] = materials;
}

void Model::ReadMesh(wstring folder, wstring file)
{
	wstring tempStr = folder + file;
#if USING_ICLONABLE
	if (meshDataMap.count(tempStr) < 1)
		LoadMesh(folder, file);
	else
	{
		MeshData data = meshDataMap[tempStr];

		for (size_t i = 0; i < data.Bones.size(); i++)
		{
			ModelBone* bone = NULL;
			data.Bones[i]->Clone((void**)&bone);

			bones.push_back(bone);
		}

		for (size_t i = 0; i < data.Meshes.size(); i++)
		{
			ModelMesh* mesh = NULL;
			data.Meshes[i]->Clone((void**)&mesh);

			meshes.push_back(mesh);
		}
	}
	BindingBone();
	BindingMesh();

	MeshData data;
	data.Bones.assign(bones.begin(), bones.end());
	data.Meshes.assign(meshes.begin(), meshes.end());
	meshDataMap[tempStr] = data;
#else
	if (meshDataMap.count(tempStr) < 1)
	{
		LoadMesh(folder, file);
		BindingBone();
		BindingMesh();

		MeshData data;
		data.Bones.assign(bones.begin(), bones.end());
		data.Meshes.assign(meshes.begin(), meshes.end());
		meshDataMap[tempStr] = data;
	}
	else
	{
		bones = meshDataMap[tempStr].Bones;
		meshes = meshDataMap[tempStr].Meshes;
	}
#endif
}

void Model::LoadMesh(wstring folder, wstring file)
{
	BinaryReader* r = new BinaryReader();
	wstring tempStr = folder + file;
	r->Open(tempStr);


	UINT count = 0;
	count = r->UInt();

	for (UINT i = 0; i < count; i++)
	{
		ModelBone* bone = new ModelBone();

		bone->index = r->Int();
		bone->name = String::ToWString(r->String());
		bone->parentIndex = r->Int();

		bone->local = r->Matrix();
		bone->global = r->Matrix();

		bones.push_back(bone);
	}

	count = r->UInt();
	for (UINT i = 0; i < count; i++)
	{
		ModelMesh* mesh = new ModelMesh();
		mesh->name = String::ToWString(r->String());
		mesh->parentBoneIndex = r->Int();

		UINT partCount = r->UInt();
		for (UINT k = 0; k < partCount; k++)
		{
			ModelMeshPart* meshPart = new ModelMeshPart();
			meshPart->parent = mesh;
			meshPart->materialName = String::ToWString(r->String());

			//VertexData
			{
				UINT count = r->UInt();
				meshPart->vertices.assign(count, ModelVertexType());

				void* ptr = (void *)&(meshPart->vertices[0]);
				r->Byte(&ptr, sizeof(ModelVertexType) * count);
			}

			//IndexData
			{
				UINT count = r->UInt();
				meshPart->indices.assign(count, UINT());

				void* ptr = (void *)&(meshPart->indices[0]);
				r->Byte(&ptr, sizeof(UINT) * count);
			}

			mesh->meshParts.push_back(meshPart);
		}//for(k)

		meshes.push_back(mesh);
	}//for(i)

	r->Close();
	SAFE_DELETE(r);
}

void Model::BindingBone()
{
	this->root = bones[0];
	for (ModelBone* bone : bones)
	{
		if (bone->parentIndex > -1)
		{
			bone->parent = bones[bone->parentIndex];
			bone->parent->childs.push_back(bone);
		}
		else
		{
			bone->parent = NULL;
		}
	}
}

void Model::BindingMesh()
{
	for (ModelMesh* mesh : meshes)
	{
		for (ModelBone* bone : bones)
		{
			if (mesh->parentBoneIndex == bone->index)
			{
				mesh->parentBone = bone;

				break;
			}
		}

		for (ModelMeshPart* part : mesh->meshParts)
		{
			for (Material* material : materials)
			{
				if (material->Name() == part->materialName)
				{
					part->material = material;

					break;
				}
			}
		}

		mesh->Binding();
	}
}
