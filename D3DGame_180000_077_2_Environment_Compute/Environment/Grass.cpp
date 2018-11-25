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
	grassCount = MaximumGrassCount;
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

		intanceData[i].Position = position;
		intanceData[i].Scale = scale;
		intanceData[i].Type = Math::Random(0, 1000) > 100 ? 0 : 1;
		intanceData[i].ShakeType = Math::Random(0, 1000) > 300 ? 0 : 1;

		intanceData[i].WindLimitFactor = Math::Random(0.0f, 1.0f);
	}

	std::sort(intanceData, (intanceData + grassCount), Grass::acompare);
	chkval.assign(intanceData, (intanceData + grassCount));
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



	{
		HRESULT hr;
		D3D11_BUFFER_DESC buffer_desc;
		ZeroMemory(&buffer_desc, sizeof(buffer_desc));
		buffer_desc.ByteWidth = MaximumGrassCount * sizeof(InstData);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		buffer_desc.StructureByteStride = sizeof(InstData);
		hr = D3D::GetDevice()->CreateBuffer(&buffer_desc, nullptr, &pBuffer);
		assert(SUCCEEDED(hr));

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavbuffer_desc;
		ZeroMemory(&uavbuffer_desc, sizeof(uavbuffer_desc));
		uavbuffer_desc.Format = DXGI_FORMAT_UNKNOWN;
		uavbuffer_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavbuffer_desc.Buffer.NumElements = MaximumGrassCount;
		hr = D3D::GetDevice()->CreateUnorderedAccessView(pBuffer, &uavbuffer_desc, &pBufferUAV);
		assert(SUCCEEDED(hr));

		D3D11_BUFFER_DESC readback_buffer_desc;
		ZeroMemory(&readback_buffer_desc, sizeof(readback_buffer_desc));
		readback_buffer_desc.ByteWidth = MaximumGrassCount * sizeof(InstData);
		readback_buffer_desc.Usage = D3D11_USAGE_STAGING;
		readback_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		readback_buffer_desc.StructureByteStride = sizeof(InstData);
		hr = D3D::GetDevice()->CreateBuffer(&readback_buffer_desc, nullptr, &pReadBackBuffer);
		assert(SUCCEEDED(hr));
	}
}

Grass::~Grass()
{
	SAFE_RELEASE(pReadBackBuffer);
	SAFE_RELEASE(pBufferUAV);
	SAFE_RELEASE(pBuffer);

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
	ac = camPos - a.Position;
	bc = camPos - b.Position;

	return (ac.x * ac.x + ac.y * ac.y + ac.z * ac.z) > (bc.x * bc.x + bc.y * bc.y + bc.z * bc.z);
}

void Grass::Update()
{
	DeltaSTime += ShakeFactor * Time::Delta();
	shader->AsScalar("DeltaSTime")->SetFloat(DeltaSTime);

	D3DXVECTOR3 cpos;
	Context::Get()->GetMainCamera()->Position(&cpos);
	{
		{
			D3D::GetDC()->UpdateSubresource(pBuffer, 0, nullptr, intanceData, 0, 0);

			for (UINT level = 2; level <= BLOCK_SIZE; level = level * 2)
			{
				shader->AsVector("g_CPosition")->GetFloatVector(cpos);
				shader->AsScalar("g_iLevel")->SetInt(level);
				shader->AsScalar("g_iLevelMask")->SetInt(level);
				shader->AsScalar("g_iWidth")->SetInt(BLOCK_SIZE);
				shader->AsScalar("g_iHeight")->SetInt(BLOCK_SIZE);

				// Sort the row data
				shader->AsUAV("GrassesData")->SetUnorderedAccessView(pBufferUAV);
				shader->Dispatch(0, 1, MaximumGrassCount / BLOCK_SIZE, 1, 1);
			}

			D3D::GetDC()->CopyResource(vertexBuffer[1], pBuffer);
		}
		//{
		//	D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
		//	D3D::GetDC()->CopyResource(pReadBackBuffer, pBuffer);
		//	if (SUCCEEDED(D3D::GetDC()->Map(pReadBackBuffer, 0, D3D11_MAP_READ, 0, &MappedResource)))
		//	{
		//		_Analysis_assume_(MappedResource.pData);
		//		assert(MappedResource.pData);
		//		memcpy(intanceData, MappedResource.pData, MaximumGrassCount * sizeof(InstData));
		//		D3D::GetDC()->Unmap(pReadBackBuffer, 0);

		//		memcpy(&chkval[0], intanceData, MaximumGrassCount * sizeof(InstData));

		//		//std::sort(intanceData, (intanceData + grassCount), Grass::acompare);
		//		//for (UINT i = 0; i < grassCount; i++)
		//		//{
		//		//	if (intanceData[i].Position != chkval[i].Position)
		//		//	{
		//		//		break;
		//		//	}
		//		//}
		//	}
		//}
	}

}

void Grass::Render()
{
	ImGui::Begin("Environment");
	{
		ImGui::Separator();
		ImGui::Text("Grass");

		ImGui::SliderInt("Grass Count", (int*)&grassCount, 0, MaximumGrassCount);

		if(ImGui::SliderFloat2("Wind Direction", (float *)&WindDirection, -1.0f, 1.0f))
			shader->AsVector("WindDirection")->SetFloatVector(WindDirection);

		if(ImGui::SliderFloat("Wind Factor", &ShakeFactor, 0.0f, 5.0f))
			shader->AsScalar("ShakeFactor")->SetFloat(ShakeFactor);

		ImGui::Separator();
	}
	ImGui::End();

	UINT stride[2] = { sizeof(Vertex), sizeof(InstData) };
	UINT offset[2] = { 0, 0 };

	D3D::GetDC()->IASetVertexBuffers(0, 2, vertexBuffer, stride, offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	
	shader->DrawInstanced(0, 0, 1, grassCount);
}
