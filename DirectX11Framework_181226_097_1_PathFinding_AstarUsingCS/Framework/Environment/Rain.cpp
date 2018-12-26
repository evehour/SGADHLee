#include "Framework.h"
#include "Rain.h"

Rain::Rain(D3DXVECTOR3 & size, UINT count)
	:particleCount((int)count)
{
	shader = new Shader(Shaders + L"054_GeometryRain.fx");
	
	rainMap = new Texture(Textures + L"Rain.png");
	shader->AsShaderResource("Map")->SetResource(rainMap->SRV());

	Vertex vertex;

	rainDatas = new VertexRain[MaximumRainCount];
	// Create Data
	{
		for (UINT i = 0; i < MaximumRainCount; i++)
		{
			D3DXVECTOR2 S;
			S.x = Math::Random(0.1f, 0.4f) * 0.1f;
			S.y = Math::Random(4.0f, 8.0f);

			D3DXVECTOR3 P;
			P.x = Math::Random(-size.x * 0.5f, size.x * 0.5f);
			P.y = Math::Random(-size.y * 0.5f, size.y * 0.5f);
			P.z = Math::Random(-size.z * 0.5f, size.z * 0.5f);

			rainDatas[i] = VertexRain(P, S);
		}
	}

	//Create Vertex Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.ByteWidth = sizeof(Vertex);
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &vertex;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &drawBuffer[0]);
		assert(SUCCEEDED(hr));
	}

	//Create Instance Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.ByteWidth = sizeof(VertexRain) * MaximumRainCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = rainDatas;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &drawBuffer[1]);
		assert(SUCCEEDED(hr));
	}

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
	for (UINT i = 0; i < 2; i++)
		SAFE_RELEASE(drawBuffer[i]);

	SAFE_DELETE_ARRAY(rainDatas);

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
		ImGui::SliderInt("Rain Count", &particleCount, 0, MaximumRainCount);
		ImGui::Separator();
	}
	ImGui::End();

	shader->AsScalar("DrawDistance")->SetFloat(rainBuffer.DrawDistance);
	shader->AsVector("RainVelocity")->SetFloatVector(rainBuffer.Velocity);
	shader->AsVector("RainColor")->SetFloatVector(rainBuffer.Color);
	shader->AsVector("Origin")->SetFloatVector(rainBuffer.OriginPosition);


	UINT stride[2] = { sizeof(Vertex), sizeof(VertexRain) };
	UINT offset[2] = { 0, 0 };

	D3D::GetDC()->IASetVertexBuffers(0, 2, drawBuffer, stride, offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	shader->DrawInstanced(0, 0, 1, particleCount);
}