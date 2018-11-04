#include "stdafx.h"
#include "Collider.h"
#include "../Objects/DebugDraw.h"
#include "../Physics/LineMake.h"

Collider::Collider()
	: vertexBuffer(NULL), indexBuffer(NULL)
	, position(0, 0, 0), scale(1.0f, 1.0f, 1.0f), rotation(0, 0, 0)
	, matParent(NULL), shader(NULL)
{
	shader = new Shader(Shaders + L"003_Color_Line.hlsl");
	worldBuffer = new WorldBuffer();
	colorBuffer = new ColorBuffer();

	D3DXMatrixIdentity(&matLocal);
	matFinal = matLocal;

	{
		compass[0] = new LineMake();
		compass[0]->AddLine(D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(50, 0, 0));
		compass[0]->SetColor(D3DXCOLOR(1, 0, 0, 1));
		compass[0]->UpdateBuffer();

		compass[1] = new LineMake();
		compass[1]->AddLine(D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0, 50, 0));
		compass[1]->SetColor(D3DXCOLOR(0, 1, 0, 1));
		compass[1]->UpdateBuffer();

		compass[2] = new LineMake();
		compass[2]->AddLine(D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0, 0, 50));
		compass[2]->SetColor(D3DXCOLOR(0, 0, 1, 1));
		compass[2]->UpdateBuffer();
	}
}

Collider::~Collider()
{
	for (UINT i = 0; i < 3;i++)
		SAFE_DELETE(compass[i]);

	SAFE_DELETE(shader);
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);
	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(colorBuffer);
}

void Collider::Update()
{
	forward = D3DXVECTOR3(matFinal._31, matFinal._32, matFinal._33);
	up = D3DXVECTOR3(matFinal._21, matFinal._22, matFinal._23);
	right = D3DXVECTOR3(matFinal._11, matFinal._12, matFinal._13);

	for (UINT i = 0; i < 3;i++)
	{
		compass[i]->SetWorld(matFinal);
		compass[i]->Update();
	}
}

void Collider::Render()
{
	if (vertexBuffer)
	{
		UINT stride = sizeof(VertexColor);
		UINT offset = 0;

		D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		colorBuffer->SetPSBuffer(10);

		worldBuffer->SetMatrix(matFinal);
		worldBuffer->SetVSBuffer(1);

		shader->Render();
		D3D::GetDC()->DrawIndexed(indexCount, 0, 0);

		for (UINT i = 0; i < 3;i++)
			compass[i]->Render();
	}
}

void Collider::UpdateData()
{
}

bool Collider::IsCollision(Collider * other)
{
	return false;
}

void Collider::CreateBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;

	//1. Vertex Buffer
	{
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexColor) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = vertexData;

		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//2. Index Buffer
	{
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = indexData;

		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}
