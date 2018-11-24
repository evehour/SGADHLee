#include "stdafx.h"
#include "Rain.h"

Rain::Rain(D3DXVECTOR3 & size, UINT count)
	:particleCount((int)count)
{
	shader = new Shader(Shaders + L"054_GeometryRain.fx");
	
	rainMap = new Texture(Textures + L"Rain.png");
	shader->AsShaderResource("Map")->SetResource(rainMap->SRV());


	VertexRain* vertices = new VertexRain[150000];

	// Create Data
	{
		for (UINT i = 0; i < 150000; i++)
		{
			D3DXVECTOR2 S;
			S.x = Math::Random(0.05f, 0.1f);
			S.y = Math::Random(1.0f, 2.0f);

			D3DXVECTOR3 P;
			P.x = Math::Random(-size.x * 0.5f, size.x * 0.5f);
			P.y = Math::Random(-size.y * 0.5f, size.y * 0.5f);
			P.z = Math::Random(-size.z * 0.5f, size.z * 0.5f);

			vertices[i] = VertexRain(P, S);
		}
	}

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexRain) * 150000;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}
	SAFE_DELETE_ARRAY(vertices);

	rainBuffer.AreaSize = size;
	rainBuffer.OriginPosition = size * 0.5f;
	rainBuffer.Velocity = D3DXVECTOR3(0, -100, 0);

	shader->AsVector("RainArea")->SetFloatVector(rainBuffer.AreaSize);
	shader->AsVector("RainVelocity")->SetFloatVector(rainBuffer.Velocity);
	shader->AsVector("RainColor")->SetFloatVector(rainBuffer.Color);
	shader->AsVector("Origin")->SetFloatVector(rainBuffer.OriginPosition);
	shader->AsScalar("DrawDistance")->SetFloat(rainBuffer.DrawDistance);
}

Rain::~Rain()
{
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE(shader);
	SAFE_DELETE(rainMap);
}

void Rain::Update()
{
	// 1000 °öÇÑ°Ç ÃÊ´ÜÀ§·Î ¹Ù²Û°Å
	rainBuffer.Time = Time::Get()->Running() * 1000.0f / 750.0f;
	shader->AsScalar("dTime")->SetFloat(rainBuffer.Time);
}

void Rain::Render()
{
	ImGui::Begin("Environment");
	{
		ImGui::Separator();
		ImGui::Text("Rain");
		ImGui::SliderFloat("DrawDistance", &rainBuffer.DrawDistance, 0, 1000);
		ImGui::SliderFloat3("Velocity", (float*)&rainBuffer.Velocity, -200, 200);
		ImGui::ColorEdit3("Color", (float*)&rainBuffer.Color);
		ImGui::DragFloat3("Area Position", (float*)&rainBuffer.OriginPosition);
		ImGui::SliderInt("Rain Count", &particleCount, 0, 150000);
		ImGui::Separator();
	}
	ImGui::End();

	shader->AsScalar("DrawDistance")->SetFloat(rainBuffer.DrawDistance);
	shader->AsVector("RainVelocity")->SetFloatVector(rainBuffer.Velocity);
	shader->AsVector("RainColor")->SetFloatVector(rainBuffer.Color);
	shader->AsVector("Origin")->SetFloatVector(rainBuffer.OriginPosition);


	UINT stride = sizeof(VertexRain);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	shader->Draw(0, 0, particleCount);
}