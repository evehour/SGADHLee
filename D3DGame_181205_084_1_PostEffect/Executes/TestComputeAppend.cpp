#include "stdafx.h"
#include "TestComputeAppend.h"


TestComputeAppend::TestComputeAppend()
	: dataSize(32)
{
	shader = new Shader(Shaders + L"055_ComputeShader_Append2.fx", false);

	vector<int> data;
	for (int i = 0; i < (int)dataSize; i++)
		data.push_back(i);

	input = new CsAppend(sizeof(int), dataSize, &data[0]);
	output = new CsAppend(sizeof(Data), dataSize, NULL);

	ExecuteCs();
}

TestComputeAppend::~TestComputeAppend()
{
	SAFE_DELETE(input);
	SAFE_DELETE(output);
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
	//UINT counters[2] = { dataSize,0 };
	//ID3D11UnorderedAccessView* uavs[2] = { input->UAV(), output->UAV() };
	//D3D::GetDC()->CSSetUnorderedAccessViews(0, 2, uavs, counters);

	//위와 동일한 코드
	shader->AsUAV("Input")->SetUnorderedAccessView(input->UAV(), dataSize);
	shader->AsUAV("Output")->SetUnorderedAccessView(output->UAV(), 0);

	shader->Dispatch(0, 0, 256, 1, 1);

	vector<Data> result(dataSize);
	output->Read(&result[0]);


	FILE* file = fopen("Result2.txt", "w");
	
	for (UINT i = 0; i < dataSize; i++)
	{
		fprintf(file, "%d, %d, %d\n",
			result[i].Id, result[i].Value, result[i].Sum
		);
	}

	fclose(file);
}
