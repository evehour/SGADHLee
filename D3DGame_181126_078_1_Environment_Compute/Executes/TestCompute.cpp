#include "stdafx.h"
#include "TestCompute.h"


TestCompute::TestCompute()
	: dataSize(20)
{
	shader = new Shader(Shaders + L"055_ComputeShader_Add.fx", false);

	vector<Data> dataA(dataSize);
	vector<Data> dataB(dataSize);

	for (int i = 0; i < dataSize; i++)
	{
		dataA[i].V1 = D3DXVECTOR3(i, i, i);
		dataA[i].V2 = D3DXVECTOR2(i, 0.0f);

		dataB[i].V1 = D3DXVECTOR3(-i, i, i);
		dataB[i].V2 = D3DXVECTOR2(i, -i);
	}

	HRESULT hr;
	//Create input Buffer
	{
		D3D11_BUFFER_DESC inputDesc;
		inputDesc.Usage = D3D11_USAGE_DEFAULT;
		inputDesc.ByteWidth = sizeof(Data) * dataSize;
		inputDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		inputDesc.CPUAccessFlags = 0;
		inputDesc.StructureByteStride = sizeof(Data); // 중요! 바이트어레이는 상관없지만, 스트럭쳐버퍼를 쓰려면 이걸 명시해줘야함. 스트럭쳐버퍼 1개의 크기를 의미.
		inputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 스트럭쳐형태로 이 버퍼를 사용한다는 의미.

		D3D11_SUBRESOURCE_DATA data1;
		data1.pSysMem = &dataA[0];

		ID3D11Buffer* buffer1 = NULL;
		hr = D3D::GetDevice()->CreateBuffer(&inputDesc, &data1, &buffer1);
		assert(SUCCEEDED(hr));

		D3D11_SUBRESOURCE_DATA data2;
		data2.pSysMem = &dataB[0];

		ID3D11Buffer* buffer2 = NULL;
		hr = D3D::GetDevice()->CreateBuffer(&inputDesc, &data2, &buffer2);
		assert(SUCCEEDED(hr));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;// Raw data로 할때는 포맷을 반드시 정해줘야 함. 스트럭쳐는 상관 없음.
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX; // 스트럭트버퍼는 요걸루다가
		srvDesc.BufferEx.FirstElement = 0;
		srvDesc.BufferEx.Flags = 0;
		srvDesc.BufferEx.NumElements = dataSize;

		hr = D3D::GetDevice()->CreateShaderResourceView(buffer1, &srvDesc, &srv1);
		assert(SUCCEEDED(hr));
		hr = D3D::GetDevice()->CreateShaderResourceView(buffer2, &srvDesc, &srv2);
		assert(SUCCEEDED(hr));

		SAFE_RELEASE(buffer1);
		SAFE_RELEASE(buffer2);
	}


	// Create Output
	{
		D3D11_BUFFER_DESC outputDesc;
		outputDesc.Usage = D3D11_USAGE_DEFAULT;
		outputDesc.ByteWidth = sizeof(Data) * dataSize;
		outputDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		outputDesc.CPUAccessFlags = 0;
		outputDesc.StructureByteStride = sizeof(Data);
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
		uavDesc.Buffer.NumElements = dataSize;

		// 묶을 때, 쉐이더 입력 버퍼로 묶으면 SO처럼 쓸수 있...나?
		hr = D3D::GetDevice()->CreateUnorderedAccessView(outputBuffer, &uavDesc, &uav);
		assert(SUCCEEDED(hr));
	}

	ExecuteCs();
}

TestCompute::~TestCompute()
{
	SAFE_DELETE(shader);
}

void TestCompute::Update()
{

}

void TestCompute::PreRender()
{

}

void TestCompute::Render()
{

}

void TestCompute::PostRender()
{

}

void TestCompute::ExecuteCs()
{
	shader->AsShaderResource("Input1")->SetResource(srv1);
	shader->AsShaderResource("Input2")->SetResource(srv2);
	shader->AsUAV("Output")->SetUnorderedAccessView(uav);

	shader->Dispatch(0, 0, 64, 1, 1);

	FILE* file = fopen("Result.txt", "w");
	D3D::GetDC()->CopyResource(outputDebugBuffer, outputBuffer);
	
	D3D11_MAPPED_SUBRESOURCE mapped;
	D3D::GetDC()->Map(outputDebugBuffer, 0, D3D11_MAP_READ, 0, &mapped);
	{
		Data* dataView = reinterpret_cast<Data*>(mapped.pData); // 보이드형의 포인터를 캐스팅할 때는 reinterpret를 쓰자.
		for (int i = 0; i < dataSize; i++)
		{
			fprintf(file, "V1 : (%f, %f, %f)", dataView[i].V1.x, dataView[i].V1.y, dataView[i].V1.z);
			fprintf(file, " - V2 : (%f, %f)\n", dataView[i].V2.x, dataView[i].V2.y);
		}
	}
	D3D::GetDC()->Unmap(outputDebugBuffer, 0);

	fclose(file);
}
