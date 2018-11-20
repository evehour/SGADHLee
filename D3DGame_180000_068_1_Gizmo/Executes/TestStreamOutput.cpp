#include "stdafx.h"
#include "TestStreamOutput.h"


TestStreamOutput::TestStreamOutput(ExecuteValues * values)
	: Execute(values)
	, bFirstRender(true)
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

	{
		// Stream Output의 대상이 되는 GS는 아래와 같이 리플렉션이 아니라 자신이
		// 직접 넣는 것이기 때문에 D3DXVECTOR3를 주므로 크기를 맞춰줘야 한다.
		shader1 = new Shader(Shaders + L"801_StreamOutput.hlsl", "VS", "");
		D3D11_SO_DECLARATION_ENTRY pDecl[] =
		{
			{ 0, "POSITION", 0, 0, 3, 0 },
			{ 0, "COLOR", 0, 0, 4, 0 },
		};
		shader1->CreateGSWithSO(
			pDecl,
			sizeof(pDecl) / sizeof(D3D11_SO_DECLARATION_ENTRY),
			"gsStreamOut"
		);

		shader2 = new Shader(Shaders + L"801_StreamOutput.hlsl", "VSPassThrough", "PS");
		shader2->CreateGS("GS");

		// 스트림에만 출력을 하고 랜더링 하지 않는 작업에서는
		// 깊이버퍼(마스크는 제로)를 꺼야 한다.
		depthStencilState[0] = new DepthStencilState();
		depthStencilState[1] = new DepthStencilState();
		depthStencilState[1]->DepthEnable(false);
		depthStencilState[1]->DepthWriteEnable(D3D11_DEPTH_WRITE_MASK_ZERO);
	}


}

TestStreamOutput::~TestStreamOutput()
{
	for (UINT i = 0; i < 2; i++)
	{
		SAFE_DELETE(depthStencilState[i]);
	}

	SAFE_DELETE(shader2);
	SAFE_DELETE(shader1);

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

	depthStencilState[1]->OMSetDepthStencilState();
	shader1->Render();

	if (bFirstRender)
		D3D::GetDC()->Draw(3, 0);
	else
		D3D::GetDC()->DrawAuto();
	depthStencilState[0]->OMSetDepthStencilState();

	ID3D11Buffer *bufferArray[1] = { 0 };
	D3D::GetDC()->SOSetTargets(1, bufferArray, &offset);

	std::swap(drawVBuffer, SOVBuffer);
	values->ViewProjection->SetGSBuffer(0);

	shader2->Render();
	D3D::GetDC()->DrawAuto();
	D3D::GetDC()->GSSetShader(NULL, NULL, 0);

	bFirstRender = false;
}

void TestStreamOutput::PostRender()
{

}


