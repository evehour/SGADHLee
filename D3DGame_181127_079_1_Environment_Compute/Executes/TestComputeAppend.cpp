#include "stdafx.h"
#include "TestComputeAppend.h"


TestComputeAppend::TestComputeAppend()
	: dataSize(790)
{
	shader = new Shader(Shaders + L"055_ComputeShader_Append2.fx", false);

	vector<int> data;
	for (int i = 0; i < dataSize; i++)
		data.push_back(i);

	input = new CsAppend(sizeof(int), dataSize, &data[0]);
	output = new CsAppend(sizeof(Data), dataSize, NULL);

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
	shader->AsUAV("Input")->SetUnorderedAccessView(input->UAV());
	shader->AsUAV("Output")->SetUnorderedAccessView(output->UAV());

	shader->Dispatch(0, 0, 2, 1, 1);

	vector<Data> result(dataSize);
	output->Read(&result[0]);


	FILE* file = fopen("Result2.txt", "w");
	
	for (int i = 0; i < dataSize; i++)
	{
		fprintf(file, "%d, %d, %d\n",
			result[i].Id, result[i].Value, result[i].Sum
		);
	}

	fclose(file);
}
