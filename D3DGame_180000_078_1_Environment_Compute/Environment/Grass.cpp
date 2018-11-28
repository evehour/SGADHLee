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
	MaximumGrassCount = grassCount = NUM_ELEMENTS;
	shader = new Shader(Shaders + L"054_Grass.fx");

	intanceData = new InstData[NUM_ELEMENTS];

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

	// Compute shader part
	{
		HRESULT hr;
		D3D11_BUFFER_DESC buffer_desc;
		ZeroMemory(&buffer_desc, sizeof(buffer_desc));
		buffer_desc.ByteWidth = NUM_ELEMENTS * sizeof(InstData);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		buffer_desc.StructureByteStride = sizeof(InstData);
		hr = D3D::GetDevice()->CreateBuffer(&buffer_desc, nullptr, &pBuffer1);
		assert(SUCCEEDED(hr));
		hr = D3D::GetDevice()->CreateBuffer(&buffer_desc, nullptr, &pBuffer2);
		assert(SUCCEEDED(hr));

		// Create the Shader Resource View for the Buffers
		// This is used for reading the buffer during the transpose
		D3D11_SHADER_RESOURCE_VIEW_DESC srvbuffer_desc;
		ZeroMemory(&srvbuffer_desc, sizeof(srvbuffer_desc));
		srvbuffer_desc.Format = DXGI_FORMAT_UNKNOWN;
		srvbuffer_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvbuffer_desc.Buffer.ElementWidth = NUM_ELEMENTS;
		hr = D3D::GetDevice()->CreateShaderResourceView(pBuffer1, &srvbuffer_desc, &pBuffer1SRV);
		assert(SUCCEEDED(hr));
		hr = D3D::GetDevice()->CreateShaderResourceView(pBuffer2, &srvbuffer_desc, &pBuffer2SRV);
		assert(SUCCEEDED(hr));

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavbuffer_desc;
		ZeroMemory(&uavbuffer_desc, sizeof(uavbuffer_desc));
		uavbuffer_desc.Format = DXGI_FORMAT_UNKNOWN;
		uavbuffer_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavbuffer_desc.Buffer.NumElements = NUM_ELEMENTS;
		hr = D3D::GetDevice()->CreateUnorderedAccessView(pBuffer1, &uavbuffer_desc, &pBuffer1UAV);
		assert(SUCCEEDED(hr));
		hr = D3D::GetDevice()->CreateUnorderedAccessView(pBuffer2, &uavbuffer_desc, &pBuffer2UAV);
		assert(SUCCEEDED(hr));

		D3D11_BUFFER_DESC readback_buffer_desc;
		ZeroMemory(&readback_buffer_desc, sizeof(readback_buffer_desc));
		readback_buffer_desc.ByteWidth = NUM_ELEMENTS * sizeof(InstData);
		readback_buffer_desc.Usage = D3D11_USAGE_STAGING;
		readback_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		readback_buffer_desc.StructureByteStride = sizeof(InstData);
		hr = D3D::GetDevice()->CreateBuffer(&readback_buffer_desc, nullptr, &pReadBackBuffer);
		assert(SUCCEEDED(hr));
	}
}

Grass::~Grass()
{
	{
		SAFE_RELEASE(pReadBackBuffer);
		SAFE_RELEASE(pBuffer2UAV);
		SAFE_RELEASE(pBuffer1UAV);
		SAFE_RELEASE(pBuffer2SRV);
		SAFE_RELEASE(pBuffer1SRV);
		SAFE_RELEASE(pBuffer2);
		SAFE_RELEASE(pBuffer1);
	}

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
		UINT orderBy = 1;
		{
			{
				//ID3D11ShaderResourceView* pViewnullptr = nullptr;
				//ID3D11UnorderedAccessView* pUAVnullptr = nullptr;
				D3D::GetDC()->UpdateSubresource(pBuffer1, 0, nullptr, intanceData, 0, 0);

				for (UINT level = 2; level <= BLOCK_SIZE; level = level * 2)
				{
					shader->AsScalar("g_iLevel")->SetInt(level);
					shader->AsScalar("g_iLevelMask")->SetInt(level);
					shader->AsScalar("g_iWidth")->SetInt(MATRIX_WIDTH);
					shader->AsScalar("g_iHeight")->SetInt(MATRIX_HEIGHT);

					// Sort the row data
					shader->AsUAV("Datas")->SetUnorderedAccessView(pBuffer1UAV);
					shader->Dispatch(orderBy, 0, (NUM_ELEMENTS / BLOCK_SIZE) < 1 ? 1 : (NUM_ELEMENTS / BLOCK_SIZE), 1, 1);
				}

				for (UINT level = (BLOCK_SIZE * 2); level <= NUM_ELEMENTS; level = level * 2)
				{
					shader->AsScalar("g_iLevel")->SetInt((level / BLOCK_SIZE));
					shader->AsScalar("g_iLevelMask")->SetInt((level & ~NUM_ELEMENTS) / BLOCK_SIZE);
					shader->AsScalar("g_iWidth")->SetInt(MATRIX_WIDTH);
					shader->AsScalar("g_iHeight")->SetInt(MATRIX_HEIGHT);

					// Transpose the data from buffer 1 into buffer 2
					//shader->AsShaderResource("Inputs")->SetResource(pViewnullptr);
					shader->AsUAV("Datas")->SetUnorderedAccessView(pBuffer2UAV);
					shader->AsShaderResource("Inputs")->SetResource(pBuffer1SRV);
					shader->Dispatch(orderBy, 1, MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE, MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE, 1);

					// Sort the transposed column data
					shader->Dispatch(orderBy, 0, NUM_ELEMENTS / BLOCK_SIZE, 1, 1);

					shader->AsScalar("g_iLevel")->SetInt(BLOCK_SIZE);
					shader->AsScalar("g_iLevelMask")->SetInt(level);
					shader->AsScalar("g_iWidth")->SetInt(MATRIX_HEIGHT);
					shader->AsScalar("g_iHeight")->SetInt(MATRIX_WIDTH);

					// Transpose the data from buffer 2 back into buffer 1
					//shader->AsShaderResource("Inputs")->SetResource(pViewnullptr);
					shader->AsUAV("Datas")->SetUnorderedAccessView(pBuffer1UAV);
					shader->AsShaderResource("Inputs")->SetResource(pBuffer2SRV);
					shader->Dispatch(orderBy, 1, MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE, MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE, 1);

					// Sort the row data
					shader->Dispatch(orderBy, 0, (NUM_ELEMENTS / BLOCK_SIZE) < 1 ? 1 : (NUM_ELEMENTS / BLOCK_SIZE), 1, 1);
				}

				//shader->AsShaderResource("Inputs")->SetResource(pViewnullptr);
				//shader->AsUAV("Datas")->SetUnorderedAccessView(pUAVnullptr);

				D3D::GetDC()->CopyResource(vertexBuffer[1], pBuffer1);
				//{
				//	D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
				//	D3D::GetDC()->CopyResource(pReadBackBuffer, pBuffer1);
				//	if (SUCCEEDED(D3D::GetDC()->Map(pReadBackBuffer, 0, D3D11_MAP_READ, 0, &MappedResource)))
				//	{
				//		_Analysis_assume_(MappedResource.pData);
				//		assert(MappedResource.pData);
				//		if (orderBy == 0)
				//			memcpy(&chkval[0], MappedResource.pData, NUM_COUNT * sizeof(UINT));
				//		else if (orderBy == 1)
				//			memcpy_s(&chkval[0], NUM_COUNT * sizeof(UINT), (((UINT*)MappedResource.pData) + (NUM_ELEMENTS - NUM_COUNT)), NUM_COUNT * sizeof(UINT));
				//		D3D::GetDC()->Unmap(pReadBackBuffer, 0);
				//	}
				//}
			}
		}
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
