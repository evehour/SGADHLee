#include "stdafx.h"
#include "Collider.h"
#include "../Objects/DebugDraw.h"

Collider::Collider()
	: vertexBuffer(NULL), indexBuffer(NULL)
	, position(0, 0, 0), scale(1.0f, 1.0f, 1.0f), rotation(0, 0, 0)
{
	shader = new Shader(Shaders + L"003_Color_Line.hlsl");
	worldBuffer = new WorldBuffer();
	colorBuffer = new ColorBuffer();
}

Collider::~Collider()
{
	SAFE_DELETE(shader);
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);
	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(colorBuffer);
}

void Collider::Update(D3DXMATRIX* matParent)
{
	D3DXMATRIX S, R;
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
	D3DXMatrixRotationYawPitchRoll(&R, rotation.y, rotation.x, rotation.z);
	matLocal = S * R;
	matLocal._41 = position.x; matLocal._42 = position.y; matLocal._43 = position.z;
	if (matParent)
	{
		matFinal = matLocal * (*matParent);
	}
	else
	{
		matFinal = matLocal;
	}
	worldBuffer->SetMatrix(matFinal);
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
		worldBuffer->SetVSBuffer(1);
		shader->Render();
		D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
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
