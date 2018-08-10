#include "stdafx.h"
#include "DrawTexture.h"

DrawTexture::DrawTexture(ExecuteValues * values)
	: Execute(values)
	, vertexCount(4 * 6), indexCount(6 * 6)
{
	shader = new Shader(Shaders + L"004_Texture.hlsl");
	worldBuffer = new WorldBuffer();

	colorBuffer = new ColorBuffer();

	vertices = new VertexTexture[vertexCount];
	// front
	vertices[ 0].Position = D3DXVECTOR3(-0.5f, -0.5f, -0.5f);
	vertices[ 1].Position = D3DXVECTOR3(-0.5f,  0.5f, -0.5f);
	vertices[ 2].Position = D3DXVECTOR3( 0.5f, -0.5f, -0.5f);
	vertices[ 3].Position = D3DXVECTOR3( 0.5f,  0.5f, -0.5f);
	
	// right
	vertices[ 4].Position = D3DXVECTOR3( 0.5f, -0.5f, -0.5f);
	vertices[ 5].Position = D3DXVECTOR3( 0.5f,  0.5f, -0.5f);
	vertices[ 6].Position = D3DXVECTOR3( 0.5f, -0.5f,  0.5f);
	vertices[ 7].Position = D3DXVECTOR3( 0.5f,  0.5f,  0.5f);

	// left
	vertices[ 8].Position = D3DXVECTOR3(-0.5f, -0.5f,  0.5f);
	vertices[ 9].Position = D3DXVECTOR3(-0.5f,  0.5f,  0.5f);
	vertices[10].Position = D3DXVECTOR3(-0.5f, -0.5f, -0.5f);
	vertices[11].Position = D3DXVECTOR3(-0.5f,  0.5f, -0.5f);

	// back
	vertices[12].Position = D3DXVECTOR3( 0.5f, -0.5f,  0.5f);
	vertices[13].Position = D3DXVECTOR3( 0.5f,  0.5f,  0.5f);
	vertices[14].Position = D3DXVECTOR3(-0.5f, -0.5f,  0.5f);
	vertices[15].Position = D3DXVECTOR3(-0.5f,  0.5f,  0.5f);

	// up
	vertices[16].Position = D3DXVECTOR3( 0.5f,  0.5f, -0.5f);
	vertices[17].Position = D3DXVECTOR3(-0.5f,  0.5f, -0.5f);
	vertices[18].Position = D3DXVECTOR3( 0.5f,  0.5f,  0.5f);
	vertices[19].Position = D3DXVECTOR3(-0.5f,  0.5f,  0.5f);

	// down
	vertices[20].Position = D3DXVECTOR3( 0.5f, -0.5f,  0.5f);
	vertices[21].Position = D3DXVECTOR3(-0.5f, -0.5f,  0.5f);
	vertices[22].Position = D3DXVECTOR3( 0.5f, -0.5f, -0.5f);
	vertices[23].Position = D3DXVECTOR3(-0.5f, -0.5f, -0.5f);

	for (int i = 0; i < 6; i++)
	{
		vertices[i * 4 + 0].Uv = D3DXVECTOR2(0, 1);
		vertices[i * 4 + 1].Uv = D3DXVECTOR2(0, 0);
		vertices[i * 4 + 2].Uv = D3DXVECTOR2(1, 1);
		vertices[i * 4 + 3].Uv = D3DXVECTOR2(1, 0);
		m_fFileName[0][i] = bind(&DrawTexture::SetTextureFileName, this, placeholders::_1, placeholders::_2, placeholders::_3);
		m_fFileName[1][i] = bind(&DrawTexture::SetTextureFileName, this, placeholders::_1, placeholders::_2, placeholders::_3);
	}

	indices = new UINT[indexCount]{
		0, 1, 2, 2, 1, 3,
		4, 5, 6, 6, 5, 7,
		8, 9, 10, 10, 9, 11,
		12, 13, 14, 14, 13, 15,
		16, 17, 18, 18, 17, 19,
		20, 21, 22, 22, 21, 23
	};

	
	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = indices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}

DrawTexture::~DrawTexture()
{
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);

	SAFE_DELETE(colorBuffer);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);
}

void DrawTexture::Update()
{
}

void DrawTexture::PreRender()
{
}

void DrawTexture::Render()
{
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	colorBuffer->SetPSBuffer(0);
	worldBuffer->SetVSBuffer(1);
	shader->Render();

	for (int i = 0; i < FACE_NUMBER_MAX; i++)
	{
		D3D::GetDC()->PSSetShaderResources(0, 1, &srv[0][i]);
		D3D::GetDC()->DrawIndexed(6, i * 6, 0);
		D3D::GetDC()->PSSetShaderResources(0, 1, &srv[1][i]);
		D3D::GetDC()->DrawIndexed(6, i * 6, 0);
	}
	
}
//IA -> Input Assembly		9
//VS -> Vertex Shader		9
/*
HS
TS		tessellation		11
DS
*/
//GS -> Geometry Shader		10
//----------------------------
//PS -> Pixel Shader  2D


void DrawTexture::PostRender()
{
	ImGui::Begin("Color");
	{
		ImGui::SliderFloat3("Color", (float *)&colorBuffer->Data.Color, 0, 1);
	}
	ImGui::End();
}

void DrawTexture::ResizeScreen()
{
}

void DrawTexture::SetTextureFileName(FACE_NUMBER faceNumber, int textureNumber, wstring fileName)
{
	//Load SRV
	{
		HRESULT hr = D3DX11CreateShaderResourceViewFromFile(D3D::GetDevice(), fileName.c_str(), NULL, NULL, &srv[textureNumber][faceNumber], NULL);
		assert(SUCCEEDED(hr));
	}
}

