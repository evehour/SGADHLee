#include "Framework.h"
#include "Render2D.h"
#include "./Viewer/Freedom.h"
#include "./Viewer/Viewport.h"
#include "./Viewer/Perspective.h"

Render2D::Render2D(UINT pass, float width, float height)
	: pass(pass), position(0, 0), scale(1, 1)
	, screenWidth(width), screenHeight(height), bCenterPosition(false)
{
	shader = new Shader(Shaders + L"990_Render2D.fx", false);

	//Create Ortho Matrix
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		if (screenWidth == 0.0f)
			screenWidth = desc.Width;
		if (screenHeight == 0.0f)
			screenHeight = desc.Height;

		//D3DXMatrixOrthoOffCenterLH(&orthographic, 0, desc.Width, 0, desc.Height, -1, 1);
		D3DXMatrixOrthoOffCenterLH(&orthographic, 0, screenWidth, 0, screenHeight, -1, 1);
		//D3DXMatrixOrthoLH(&orthographic, screenWidth, screenHeight, -1, 1);
	}

	//Create VertexBuffer
	{
		VertexTexture* vertices = new VertexTexture[6];
		vertices[0].Position = D3DXVECTOR3(-0.5f, -0.5f, 0.0f);
		vertices[1].Position = D3DXVECTOR3(-0.5f, 0.5f, 0.0f);
		vertices[2].Position = D3DXVECTOR3(0.5f, -0.5f, 0.0f);
		vertices[3].Position = D3DXVECTOR3(0.5f, -0.5f, 0.0f);
		vertices[4].Position = D3DXVECTOR3(-0.5f, 0.5f, 0.0f);
		vertices[5].Position = D3DXVECTOR3(0.5f, 0.5f, 0.0f);

		vertices[0].Uv = D3DXVECTOR2(0, 1);
		vertices[1].Uv = D3DXVECTOR2(0, 0);
		vertices[2].Uv = D3DXVECTOR2(1, 1);
		vertices[3].Uv = D3DXVECTOR2(1, 1);
		vertices[4].Uv = D3DXVECTOR2(0, 0);
		vertices[5].Uv = D3DXVECTOR2(1, 0);

		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * 6;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));

		SAFE_DELETE_ARRAY(vertices);
	}

	D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(0, 0, -1), &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0));

	shader->AsMatrix("View")->SetMatrix(view);
	shader->AsMatrix("Projection")->SetMatrix(orthographic);

	UpdateWorld();
}

Render2D::~Render2D()
{
	SAFE_DELETE(shader);

	SAFE_RELEASE(vertexBuffer);
}

void Render2D::Position(const float& x, const float& y)
{
	Position(D3DXVECTOR2(x, y));
}

void Render2D::Position(const D3DXVECTOR2& vec)
{
	position = vec;

	UpdateWorld();
}

void Render2D::Scale(const float& x, const float& y)
{
	Scale(D3DXVECTOR2(x, y));
}

void Render2D::Scale(const D3DXVECTOR2& vec)
{
	scale = vec;

	UpdateWorld();
}

void Render2D::Update()
{

}

void Render2D::Render()
{
	// �����  ����
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->Draw(0, pass, 6);

	//D3D::Get()->SetRenderTarget();
}

void Render2D::SRV(ID3D11ShaderResourceView * srv)
{
	shader->AsShaderResource("Map")->SetResource(srv);
}

void Render2D::UpdateWorld()
{
	D3DXMATRIX S, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, 1);
	//D3DXMatrixTranslation(&T, position.x + scale.x * 0.5f, position.y + scale.y * 0.5f, 0);
	D3DXMatrixTranslation(&T, position.x + (bCenterPosition ? 0 : scale.x * 0.5f), position.y + (bCenterPosition ? 0 : scale.y * 0.5f), 0);

	world = S * T;
	shader->AsMatrix("World")->SetMatrix(world);
}
