#include "stdafx.h"
#include "Base3DParticleInstancer.h"

#include "Base3DParticleInstance.h"

Base3DParticleInstancer::Base3DParticleInstancer(wstring shaderFName, wstring textureFName)
	: shaderFName(shaderFName), textureFName(textureFName)
	, instanceBuffer(NULL), particleShader(NULL)
{
	this->position = { 0,0,0 };
	this->scale = { 1,1,1 };
	D3DXQuaternionIdentity(&this->rotation);
	this->size = D3DXVECTOR2(0.5f, 0.5f);
	D3DXMatrixIdentity(&this->world);

	LoadContent();
}

Base3DParticleInstancer::~Base3DParticleInstancer()
{
	for (UINT i = 0; i < 2; i++)
	{
		SAFE_DELETE(blendState[i]);
		SAFE_DELETE(depthStencilState[i]);
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

	if (instanceTransformMatrices.size() < 1)
		return;

	if (instanceBuffer == NULL || instanceTransformMatrices.size() != instanceCount)
		CalcVertexBuffer();

	UINT stride[2] = { sizeof(VertexTexture), sizeof(VertexMatrix) };
	UINT offset[2] = { 0, 0 };

	ID3D11Buffer* ppBuffer[2] = { vertexBuffer, instanceBuffer };
	D3D::GetDC()->IASetVertexBuffers(0, 2, ppBuffer, stride, offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3D11ShaderResourceView* view = particleTexture->GetView();
	D3D::GetDC()->PSSetShaderResources(10, 1, &view);

	particleInstanceShaderBuffer->SetVSBuffer(10);

	blendState[1]->OMSetBlendState();
	depthStencilState[1]->OMSetDepthStencilState();
	{
		particleShader->Render();
		D3D::GetDC()->DrawIndexedInstanced(6, instanceCount, 0, 0, 0);
	}
	blendState[0]->OMSetBlendState();
	depthStencilState[0]->OMSetDepthStencilState();
}

void Base3DParticleInstancer::LoadContent()
{
	particleShader = new Shader(shaderFName);
	particleTexture = new Texture(textureFName);
	particleInstanceShaderBuffer = new ParticleInstanceShaderBuffer();

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

	blendState[0] = new BlendState();
	blendState[1] = new BlendState();

	depthStencilState[0] = new DepthStencilState();
	depthStencilState[1] = new DepthStencilState();
}

void Base3DParticleInstancer::CalcVertexBuffer()
{
	if (instanceBuffer != NULL)
		SAFE_RELEASE(instanceBuffer);

	instanceCount = instanceTransformMatrices.size();
	VertexMatrix* matrice = new VertexMatrix[instanceCount];

	UINT count = 0;
	for (auto iter = instanceTransformMatrices.begin(); iter != instanceTransformMatrices.end(); ++iter)
		matrice[count++].matrix = iter->second;

	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexMatrix) * instanceCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = matrice;

	HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &instanceBuffer);
	assert(SUCCEEDED(hr));

	SAFE_DELETE_ARRAY(matrice);
}
