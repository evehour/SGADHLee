#include "stdafx.h"
#include "Base3DParticleInstancer.h"

#include "Base3DParticleInstance.h"

Base3DParticleInstancer::Base3DParticleInstancer(wstring shaderFName, wstring textureFName)
	: shaderFName(shaderFName), textureFName(textureFName)
	, instanceBuffer(NULL), particleShader(NULL)
	, instanceCount(0), instanceCountBefore(0)
{
	srand((UINT)time(0));
	this->position = { 0,0,0 };
	this->scale = { 1,1,1 };
	D3DXQuaternionIdentity(&this->rotation);
	this->size = D3DXVECTOR2(0.5f, 0.5f);
	D3DXMatrixIdentity(&this->world);

	LoadContent();
}

Base3DParticleInstancer::~Base3DParticleInstancer()
{
	for (auto iter = instances.begin(); iter != instances.end();++iter)
	{
		IntanceDataBuffer tmp = (*iter);
		SAFE_DELETE(tmp.instance);
	}

	for (UINT i = 0; i < 2; i++)
	{
		SAFE_DELETE(blendState[i]);
		SAFE_DELETE(depthStencilState[i]);
		SAFE_DELETE(rasterizerState[i]);
	}

	SAFE_RELEASE(instanceBuffer);
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
	SAFE_DELETE(particleInstanceShaderBuffer);
	SAFE_DELETE(particleTexture);
	SAFE_DELETE(particleShader);
}

void Base3DParticleInstancer::Render()
{
	this->Render(particleShader);
}

void Base3DParticleInstancer::Render(Shader * shader)
{
	D3DXMATRIX mS, mR, mT;
	D3DXMatrixScaling(&mS, scale.x, scale.y, scale.z);
	D3DXMatrixRotationQuaternion(&mR, &rotation);
	D3DXMatrixTranslation(&mT, position.x, position.y, position.z);

	world = mS * mR * mT;
	worldBuffer->SetMatrix(world);

	if (instances.size() < 1)
		return;

	if (instanceBuffer == NULL || instances.size() != instanceCount)
		CalcVertexBuffer();

	UINT stride[2] = { sizeof(VertexTexture), sizeof(IntanceDataBuffer) };
	UINT offset[2] = { 0, 0 };

	ID3D11Buffer* ppBuffer[2] = { vertexBuffer, instanceBuffer };
	D3D::GetDC()->IASetVertexBuffers(0, 2, ppBuffer, stride, offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3D11ShaderResourceView* view = particleTexture->GetView();
	D3D::GetDC()->PSSetShaderResources(10, 1, &view);

	worldBuffer->SetVSBuffer(1);
	particleInstanceShaderBuffer->SetPSBuffer(5);

	blendState[1]->OMSetBlendState();
	depthStencilState[1]->OMSetDepthStencilState();
	rasterizerState[1]->RSSetState();
	{
		particleShader->Render();
		D3D::GetDC()->DrawIndexedInstanced(6, instanceCount, 0, 0, 0);
	}
	blendState[0]->OMSetBlendState();
	depthStencilState[0]->OMSetDepthStencilState();
	rasterizerState[0]->RSSetState();
}

void Base3DParticleInstancer::LoadContent()
{
	HRESULT hr;

	bool usingInstance = true;

	particleShader = new Shader(shaderFName, "VS", "PS", usingInstance);
	particleTexture = new Texture(textureFName);
	particleInstanceShaderBuffer = new ParticleInstanceShaderBuffer();

	//Create VertexBuffer / IndexBuffer
	{
		{
			VertexTexture* vertices = new VertexTexture[4];
			vertices[0].Position = D3DXVECTOR3(0, 0, 0);
			vertices[1].Position = D3DXVECTOR3(0, 0, 0);
			vertices[2].Position = D3DXVECTOR3(0, 0, 0);
			vertices[3].Position = D3DXVECTOR3(0, 0, 0);

			vertices[0].Uv = D3DXVECTOR2(1, 1);
			vertices[1].Uv = D3DXVECTOR2(0, 1);
			vertices[2].Uv = D3DXVECTOR2(0, 0);
			vertices[3].Uv = D3DXVECTOR2(1, 0);

			D3D11_BUFFER_DESC desc = { 0 };
			desc.Usage = D3D11_USAGE_IMMUTABLE;
			desc.ByteWidth = sizeof(VertexTexture) * 4;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA data = { 0 };
			data.pSysMem = vertices;

			hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
			assert(SUCCEEDED(hr));
			SAFE_DELETE_ARRAY(vertices);
		}

		{
			UINT* indices = new UINT[6];
			indices[0] = 0;
			indices[1] = 1;
			indices[2] = 2;
			indices[3] = 2;
			indices[4] = 3;
			indices[5] = 0;

			D3D11_BUFFER_DESC desc = { 0 };
			desc.Usage = D3D11_USAGE_IMMUTABLE;
			desc.ByteWidth = sizeof(UINT) * 6;
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

			D3D11_SUBRESOURCE_DATA data = { 0 };
			data = { 0 };
			data.pSysMem = indices;

			hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
			assert(SUCCEEDED(hr));
			SAFE_DELETE_ARRAY(indices);
		}

	}

	blendState[0] = new BlendState();
	blendState[1] = new BlendState();
	blendState[1]->BlendEnable(true);
	blendState[1]->SrcBlend(D3D11_BLEND_SRC_ALPHA);
	blendState[1]->SrcAlpha(D3D11_BLEND_SRC_ALPHA);
	blendState[1]->DestBlend(D3D11_BLEND_INV_SRC_ALPHA);
	blendState[1]->DestAlpha(D3D11_BLEND_INV_SRC_ALPHA);

	depthStencilState[0] = new DepthStencilState();
	depthStencilState[1] = new DepthStencilState();
	depthStencilState[1]->DepthEnable(false);

	rasterizerState[0] = new RasterizerState();
	rasterizerState[1] = new RasterizerState();
	rasterizerState[1]->CullMode(D3D11_CULL_NONE);

	worldBuffer = new WorldBuffer();
}

void Base3DParticleInstancer::CalcVertexBuffer()
{
	instanceCount = instances.size();

	if (instanceBuffer != NULL)
	{
		D3D11_MAPPED_SUBRESOURCE subResource;
		D3D::GetDC()->Map(instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &subResource);
		{
			memcpy(subResource.pData, &instances[0], instanceCount * sizeof(IntanceDataBuffer));
		}
		D3D::GetDC()->Unmap(instanceBuffer, 0);
	}
	else
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.ByteWidth = sizeof(IntanceDataBuffer) * instanceCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &instances[0];

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &instanceBuffer);
		assert(SUCCEEDED(hr));
	}
}

void Base3DParticleInstancer::Rotation(D3DXVECTOR3 axis, float angle)
{
	D3DXQUATERNION rot;
	axis = Math::Transform(axis, this->rotation);
	D3DXQuaternionRotationAxis(&rot, &axis, angle);

	this->rotation = rot * this->rotation;
	D3DXQuaternionNormalize(&this->rotation, &this->rotation);
}

void Base3DParticleInstancer::SetLightColor(D3DXCOLOR lightColor)
{
	particleInstanceShaderBuffer->Data.LightColor = lightColor;
}
