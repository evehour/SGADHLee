#include "stdafx.h"
#include "TestStreamOutput.h"


TestStreamOutput::TestStreamOutput()
	: bFirstRender(true)
{
	{
		VertexColor vertices[3];

		vertices[0].Position = D3DXVECTOR3(+0.0f, +0.5f, +0.0f);
		vertices[0].Color = D3DXCOLOR(1, 0, 0, 1);
		vertices[1].Position = D3DXVECTOR3(-0.5f, -0.5f, +0.0f);
		vertices[1].Color = D3DXCOLOR(0, 1, 0, 1);
		vertices[2].Position = D3DXVECTOR3(+0.5f, -0.5f, +0.0f);
		vertices[2].Color = D3DXCOLOR(0, 0, 1, 1);

		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexColor) * 3;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;
		data.SysMemPitch = sizeof(VertexColor);

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &initBuffer);
		assert(SUCCEEDED(hr));

		//desc.ByteWidth = sizeof(VertexColor) * 3 * 3;
		desc.BindFlags |= D3D11_BIND_STREAM_OUTPUT;

		hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &drawVBuffer);
		assert(SUCCEEDED(hr));

		hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &SOVBuffer);
		assert(SUCCEEDED(hr));
	}

	shader = new Shader(Shaders + L"801_StreamOutput.fx");
}

TestStreamOutput::~TestStreamOutput()
{
	SAFE_DELETE(shader);

	SAFE_RELEASE(initBuffer);
	SAFE_RELEASE(drawVBuffer);
	SAFE_RELEASE(SOVBuffer);
}

void TestStreamOutput::Update()
{

}

void TestStreamOutput::PreRender()
{

}

void TestStreamOutput::Render()
{
	UINT stride = sizeof(VertexColor);
	UINT offset = 0;

	ID3D11Buffer *pBuffers[1];
	if (bFirstRender)
		pBuffers[0] = initBuffer;
	else
		pBuffers[0] = drawVBuffer;
	
	D3D::GetDC()->IASetVertexBuffers(0, 1, pBuffers, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// Point to the correct output buffer
	pBuffers[0] = SOVBuffer;
	D3D::GetDC()->SOSetTargets(1, pBuffers, &offset);

	//shader->Draw(0, 0, 3);
	shader->DrawAuto(0, 0);

	ID3D11Buffer *bufferArray[1] = { 0 };
	D3D::GetDC()->SOSetTargets(1, bufferArray, &offset);

	std::swap(drawVBuffer, SOVBuffer);

	shader->Draw(0, 1, 300);

	bFirstRender = false;
}

void TestStreamOutput::PostRender()
{

}


