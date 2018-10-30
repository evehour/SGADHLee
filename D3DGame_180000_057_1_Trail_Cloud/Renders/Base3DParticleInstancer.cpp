#include "stdafx.h"
#include "Base3DParticleInstancer.h"

#include "Base3DParticleInstance.h"

Base3DParticleInstancer::Base3DParticleInstancer(wstring shaderFName)
{
	this->shaderFName = shaderFName;
	this->textureFName = L"";

	this->position = { 0,0,0 };
	this->scale = { 1,1,1 };
	D3DXQuaternionIdentity(&this->rotation);
	this->size = D3DXVECTOR2(0.5f, 0.5f);
	D3DXMatrixIdentity(&this->world);

	LoadContent();
}

Base3DParticleInstancer::~Base3DParticleInstancer()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
	SAFE_DELETE(shader);
}

void Base3DParticleInstancer::Render()
{
	this->Render(shader);
}

void Base3DParticleInstancer::Render(Shader * shader)
{
	D3DXMATRIX mS, mR, mT;
	D3DXMatrixScaling(&mS, scale.x, scale.y, scale.z);
	D3DXMatrixRotationQuaternion(&mR, &rotation);
	D3DXMatrixTranslation(&mT, position.x, position.y, position.z);

	world = mS * mR * mT;

	if (instanceTransformMatrices.size() < 1)
		return;
}

void Base3DParticleInstancer::LoadContent()
{
	shader = new Shader(shaderFName);

	//Create VertexBuffer / IndexBuffer
	{
		VertexTexture* vertices = new VertexTexture[4];
		vertices[0].Position = D3DXVECTOR3(0,0,0);
		vertices[1].Position = D3DXVECTOR3(0,0,0);
		vertices[2].Position = D3DXVECTOR3(0,0,0);
		vertices[3].Position = D3DXVECTOR3(0,0,0);

		vertices[0].Uv = D3DXVECTOR2(1, 1);
		vertices[1].Uv = D3DXVECTOR2(0, 1);
		vertices[2].Uv = D3DXVECTOR2(0, 0);
		vertices[3].Uv = D3DXVECTOR2(1, 0);

		UINT* indices = new UINT[6];
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 2;
		indices[4] = 3;
		indices[5] = 0;

		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * 4;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));

		desc.ByteWidth = sizeof(UINT) * 6;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		data = { 0 };
		data.pSysMem = indices;
		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);

		SAFE_DELETE_ARRAY(vertices);
		SAFE_DELETE_ARRAY(indices);
	}
}
