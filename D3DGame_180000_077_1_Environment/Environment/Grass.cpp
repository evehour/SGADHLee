#include "stdafx.h"
#include "Grass.h"
#include "./Environment/Terrain.h"
#include "./Viewer/Camera.h"

Grass::Grass(Terrain * terrain)
	: terrain(terrain)
{
	drawCount = 0;
	WindDirection = D3DXVECTOR2(1.0f, 0.0f);
	ShakeFactor = 0.0f;
	DeltaSTime = 0.0f;
	grassCount = 150000;
	shader = new Shader(Shaders + L"054_Grass.fx");

	intanceData = new InstData[grassCount];

	Vertex vertex[1];// = new Vertex;
	vertex[0].Position = D3DXVECTOR3(0, 0, 0);

	//Create Vertex Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(Vertex);
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertex;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer[0]);
		assert(SUCCEEDED(hr));
	}

	//SAFE_DELETE(vertex);

	for (UINT i = 0; i < grassCount; i++)
	{
		D3DXVECTOR2 scale;
		scale.x = 1.0f;
		scale.y = Math::Random(0.5f, 2.0f);

		D3DXVECTOR3 position;
		position.x = Math::Random(0.0f, 255.0f);
		position.z = Math::Random(0.0f, 255.0f);
		position.y = terrain->GetHeight(position.x, position.z) + scale.y * 0.5f;

		D3DXMATRIX S, T;
		D3DXMatrixScaling(&S, scale.x, scale.y, 1);
		D3DXMatrixTranslation(&T, position.x, position.y, position.z);

		intanceData[i].World = S * T;
		intanceData[i].Type = Math::Random(0, 1000) > 100 ? 0 : 1;
		intanceData[i].ShakeType = Math::Random(0, 1000) > 300 ? 0 : 1;

		intanceData[i].WindLimitFactor = Math::Random(0.0f, 1.0f);
	}

	std::sort(intanceData, (intanceData + grassCount), Grass::acompare);

	//Create Instance Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(InstData) * grassCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = intanceData;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer[1]);
		assert(SUCCEEDED(hr));
	}

	vector<wstring> textures;
	textures.push_back(Textures + L"Grass/Grass000.png");
	textures.push_back(Textures + L"Grass/Grass001.png");
	textureArray = new TextureArray(textures);

	shader->AsShaderResource("Maps")->SetResource(textureArray->SRV());
}

Grass::~Grass()
{
	SAFE_DELETE(textureArray);
	for (UINT i = 0; i < 2; i++)
		SAFE_RELEASE(vertexBuffer[i]);

	SAFE_DELETE_ARRAY(intanceData);
	SAFE_DELETE(shader);
}

bool Grass::acompare(InstData a, InstData b)
{
	D3DXVECTOR3 camPos;
	D3DXVECTOR3 ac, bc;
	Context::Get()->GetMainCamera()->Position(&camPos);
	ac = camPos - D3DXVECTOR3(a.World._41, a.World._42, a.World._43);
	bc = camPos - D3DXVECTOR3(b.World._41, b.World._42, b.World._43);

	return (ac.x * ac.x + ac.y * ac.y + ac.z * ac.z) > (bc.x * bc.x + bc.y * bc.y + bc.z * bc.z);
}

void Grass::Update()
{
	DeltaSTime += ShakeFactor * Time::Delta();
	//D3D11_MAPPED_SUBRESOURCE subResource;
	//ZeroMemory(&subResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	//HRESULT hr = D3D::GetDC()->Map
	//(
	//	vertexBuffer[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
	//);
	//assert(SUCCEEDED(hr));

	//memcpy(subResource.pData, intanceData, sizeof(InstData) * grassCount);

	//D3D::GetDC()->Unmap(vertexBuffer[1], 0);
}

void Grass::Render()
{
	ImGui::SliderInt("Grass Count", &drawCount, 0, 150000);
	ImGui::Begin("Environment");
	ImGui::Separator();
	ImGui::Text("Grass");
	ImGui::SliderFloat2("Wind Direction", (float *)&WindDirection, -1.0f, 1.0f);
	ImGui::SliderFloat("Wind Factor", &ShakeFactor, 0.0f, 5.0f);
	ImGui::Separator();
	ImGui::End();

	UINT stride[2] = { sizeof(Vertex), sizeof(InstData) };
	UINT offset[2] = { 0, 0 };

	D3D::GetDC()->IASetVertexBuffers(0, 2, vertexBuffer, stride, offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	shader->AsVector("WindDirection")->SetFloatVector(WindDirection);
	shader->AsScalar("ShakeFactor")->SetFloat(ShakeFactor);
	shader->AsScalar("DeltaSTime")->SetFloat(DeltaSTime);
	shader->DrawInstanced(0, 0, 1, grassCount);
}
