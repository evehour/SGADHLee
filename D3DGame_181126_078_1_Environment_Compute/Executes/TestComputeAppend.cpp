#include "stdafx.h"
#include "TestComputeAppend.h"


TestComputeAppend::TestComputeAppend()
	: dataSize(256)
{
	shader = new Shader(Shaders + L"055_ComputeShader_Append.fx", false);

	vector<Particle> dataA(dataSize);

	for (int i = 0; i < dataSize; i++)
	{
		dataA[i].Position.x = Math::Random(-100.0f, 100.0f);
		dataA[i].Position.y = Math::Random(-100.0f, 100.0f);
		dataA[i].Position.z = Math::Random(-100.0f, 100.0f);

		dataA[i].Velocity.x = Math::Random(-10.0f, 10.0f);
		dataA[i].Velocity.y = Math::Random(-10.0f, 10.0f);
		dataA[i].Velocity.z = Math::Random(-10.0f, 10.0f);

		dataA[i].Accelation.x = Math::Random(-5.0f, 5.0f);
		dataA[i].Accelation.y = Math::Random(-5.0f, 5.0f);
		dataA[i].Accelation.z = Math::Random(-5.0f, 5.0f);

	}

	HRESULT hr;
	//Create input Buffer
	{
		D3D11_BUFFER_DESC inputDesc;
		inputDesc.Usage = D3D11_USAGE_DEFAULT;
		inputDesc.ByteWidth = sizeof(Particle) * dataSize;
		inputDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		inputDesc.CPUAccessFlags = 0;
		inputDesc.StructureByteStride = sizeof(Particle); // �߿�! ����Ʈ��̴� ���������, ��Ʈ���Ĺ��۸� ������ �̰� ����������. ��Ʈ���Ĺ��� 1���� ũ�⸦ �ǹ�.
		inputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // ��Ʈ�������·� �� ���۸� ����Ѵٴ� �ǹ�.

		D3D11_SUBRESOURCE_DATA data1;
		data1.pSysMem = &dataA[0];

		ID3D11Buffer* buffer1 = NULL;
		hr = D3D::GetDevice()->CreateBuffer(&inputDesc, &data1, &buffer1);
		assert(SUCCEEDED(hr));



		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;// Raw data�� �Ҷ��� ������ �ݵ�� ������� ��. ��Ʈ���Ĵ� ��� ����.
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX; // ��Ʈ��Ʈ���۴� ��ɷ�ٰ�
		srvDesc.BufferEx.FirstElement = 0;
		srvDesc.BufferEx.Flags = 0;
		srvDesc.BufferEx.NumElements = dataSize;

		hr = D3D::GetDevice()->CreateShaderResourceView(buffer1, &srvDesc, &consume);
		assert(SUCCEEDED(hr));

		SAFE_RELEASE(buffer1);
	}


	// Create Output
	{
		D3D11_BUFFER_DESC outputDesc;
		outputDesc.Usage = D3D11_USAGE_DEFAULT;
		outputDesc.ByteWidth = sizeof(Particle) * dataSize;
		outputDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		outputDesc.CPUAccessFlags = 0;
		outputDesc.StructureByteStride = sizeof(Particle);
		outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		hr = D3D::GetDevice()->CreateBuffer(&outputDesc, NULL, &outputBuffer);
		assert(SUCCEEDED(hr));

		outputDesc.Usage = D3D11_USAGE_STAGING;
		outputDesc.BindFlags = 0;
		outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		hr = D3D::GetDevice()->CreateBuffer(&outputDesc, NULL, &outputDebugBuffer);
		assert(SUCCEEDED(hr));

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		uavDesc.Buffer.NumElements = dataSize;

		// ���� ��, ���̴� �Է� ���۷� ������ SOó�� ���� ��...��?
		hr = D3D::GetDevice()->CreateUnorderedAccessView(outputBuffer, &uavDesc, &append);
		assert(SUCCEEDED(hr));
	}

	ExecuteCs();
}

TestComputeAppend::~TestComputeAppend()
{
	SAFE_DELETE(shader);
}

void TestComputeAppend::Update()
{

}

void TestComputeAppend::PreRender()
{

}

void TestComputeAppend::Render()
{

}

void TestComputeAppend::PostRender()
{

}

void TestComputeAppend::ExecuteCs()
{
	shader->AsShaderResource("cInput")->SetResource(consume);
	shader->AsUAV("aOutput")->SetUnorderedAccessView(append);

	shader->Dispatch(1, 0, 16, 16, 1);

	FILE* file = fopen("Result2.txt", "w");
	D3D::GetDC()->CopyResource(outputDebugBuffer, outputBuffer);
	
	D3D11_MAPPED_SUBRESOURCE mapped;
	D3D::GetDC()->Map(outputDebugBuffer, 0, D3D11_MAP_READ, 0, &mapped);
	{
		Particle* dataView = reinterpret_cast<Particle*>(mapped.pData); // ���̵����� �����͸� ĳ������ ���� reinterpret�� ����.
		for (int i = 0; i < dataSize; i++)
		{
			fprintf(file, "V Position : (%f, %f, %f)\n", dataView[i].Position.x, dataView[i].Position.y, dataView[i].Position.z);
		}
	}
	D3D::GetDC()->Unmap(outputDebugBuffer, 0);

	fclose(file);
}
