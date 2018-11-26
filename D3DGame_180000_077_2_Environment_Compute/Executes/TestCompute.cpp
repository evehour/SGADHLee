#include "stdafx.h"
#include "TestCompute.h"


TestCompute::TestCompute()
{
	prevTime = 0.0f;

	shader = NULL;
	shader = new Shader(Shaders + L"802_ComputeShader.fx");

	NUM_COUNT = 16;

	shader->AsScalar("MaximumIndex")->SetInt(NUM_COUNT);

	buffer.resize(NUM_ELEMENTS);

	for (UINT i = 0; i < NUM_COUNT; i++)
		buffer[i] = i;

	for (UINT i = 0; i < 100000; i++)
	{
		UINT src = Math::Random(0, (int)NUM_COUNT - 1);
		UINT dst = Math::Random(0, (int)NUM_COUNT - 1);
		dst = (src == dst) ? Math::Random(0, (int)NUM_COUNT - 1) : dst;
		UINT tmpBuf;

		tmpBuf = buffer[src];
		buffer[src] = buffer[dst];
		buffer[dst] = tmpBuf;
	}

	chkval.resize(NUM_COUNT);


	{
		HRESULT hr;
		D3D11_BUFFER_DESC buffer_desc;
		ZeroMemory(&buffer_desc, sizeof(buffer_desc));
		buffer_desc.ByteWidth = NUM_ELEMENTS * sizeof(UINT);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		buffer_desc.StructureByteStride = sizeof(UINT);
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
		readback_buffer_desc.ByteWidth = NUM_ELEMENTS * sizeof(UINT);
		readback_buffer_desc.Usage = D3D11_USAGE_STAGING;
		readback_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		readback_buffer_desc.StructureByteStride = sizeof(UINT);
		hr = D3D::GetDevice()->CreateBuffer(&readback_buffer_desc, nullptr, &pReadBackBuffer);
		assert(SUCCEEDED(hr));
	}
}

TestCompute::~TestCompute()
{
	SAFE_DELETE(shader);

	{
		SAFE_RELEASE(pReadBackBuffer);
		SAFE_RELEASE(pBuffer2UAV);
		SAFE_RELEASE(pBuffer1UAV);
		SAFE_RELEASE(pBuffer2SRV);
		SAFE_RELEASE(pBuffer1SRV);
		SAFE_RELEASE(pBuffer2);
		SAFE_RELEASE(pBuffer1);
	}
}

void TestCompute::Update()
{
	long int a, b;
	a = GetTickCount();

	elapTime = Time::Get()->Running() - prevTime;
	prevTime = Time::Get()->Running();

	UINT orderBy = 0;
	{
		{
			ID3D11ShaderResourceView* pViewnullptr = nullptr;
			ID3D11UnorderedAccessView* pUAVnullptr = nullptr;
			D3D::GetDC()->UpdateSubresource(pBuffer1, 0, nullptr, &buffer[0], 0, 0);

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
				shader->AsShaderResource("Inputs")->SetResource(pViewnullptr);
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
				shader->AsShaderResource("Inputs")->SetResource(pViewnullptr);
				shader->AsUAV("Datas")->SetUnorderedAccessView(pBuffer1UAV);
				shader->AsShaderResource("Inputs")->SetResource(pBuffer2SRV);
				shader->Dispatch(orderBy, 1, MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE, MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE, 1);

				// Sort the row data
				shader->Dispatch(orderBy, 0, (NUM_ELEMENTS / BLOCK_SIZE) < 1 ? 1 : (NUM_ELEMENTS / BLOCK_SIZE), 1, 1);
			}

			shader->AsShaderResource("Inputs")->SetResource(pViewnullptr);
			shader->AsUAV("Datas")->SetUnorderedAccessView(pUAVnullptr);

			{
				D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
				D3D::GetDC()->CopyResource(pReadBackBuffer, pBuffer1);
				if (SUCCEEDED(D3D::GetDC()->Map(pReadBackBuffer, 0, D3D11_MAP_READ, 0, &MappedResource)))
				{
					_Analysis_assume_(MappedResource.pData);
					assert(MappedResource.pData);
					if (orderBy == 0)
						memcpy(&chkval[0], MappedResource.pData, NUM_COUNT * sizeof(UINT));
					else if(orderBy == 1)
						memcpy_s(&chkval[0], NUM_COUNT * sizeof(UINT), (((UINT*)MappedResource.pData) + (NUM_ELEMENTS - NUM_COUNT)), NUM_COUNT * sizeof(UINT));
					D3D::GetDC()->Unmap(pReadBackBuffer, 0);
				}
			}
		}
	}
	b = GetTickCount();
	elapTime3 = b - a;
}

void TestCompute::PreRender()
{

}

void TestCompute::Render()
{
	ImGui::Text("ETime: %d", elapTime3);
}

void TestCompute::PostRender()
{

}