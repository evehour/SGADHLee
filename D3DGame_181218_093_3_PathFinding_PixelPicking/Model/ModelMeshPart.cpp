#include "stdafx.h"
#include "ModelMeshPart.h"
#include "ModelMesh.h"
#include "ModelBone.h"

ModelMeshPart::ModelMeshPart()
	: vertexBuffer(NULL), indexBuffer(NULL)
	, pass(0)
{
}

ModelMeshPart::~ModelMeshPart()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

void ModelMeshPart::Binding()
{
	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;

	//1. Vertex Buffer
	{
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTextureNormalTangentBlend) * vertices.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &vertices[0];

		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//2. Index Buffer
	{
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indices.size();
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &indices[0];

		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}

void ModelMeshPart::Render()
{
	UINT stride = sizeof(VertexTextureNormalTangentBlend);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	UINT boneIndex = parent->ParentBoneIndex();
	material->GetShader()->AsScalar("BoneIndex")->SetInt((int)boneIndex);

	material->GetShader()->DrawIndexed(0, pass, indices.size());
}

void ModelMeshPart::Clone(void ** clone)
{
	ModelMeshPart* part = new ModelMeshPart();
	part->materialName = materialName;
	part->vertices.assign(vertices.begin(), vertices.end());
	part->indices.assign(indices.begin(), indices.end());

	*clone = part;
}
