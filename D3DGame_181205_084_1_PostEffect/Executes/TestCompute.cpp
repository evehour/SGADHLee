#include "stdafx.h"
#include "TestCompute.h"


TestCompute::TestCompute()
	: dataSize(128)
{
	shader = new Shader(Shaders + L"055_ComputeShader_Add.fx", false);

	vector<Data> dataA(dataSize);
	vector<Data> dataB(dataSize);

	for (int i = 0; i < dataSize; i++)
	{
		dataA[i].V1 = D3DXVECTOR3((float)i, (float)i, (float)i);
		dataA[i].V2 = D3DXVECTOR2((float)i, 0.0f);

		dataB[i].V1 = D3DXVECTOR3((float)(-i), (float)i, (float)i);
		dataB[i].V2 = D3DXVECTOR2((float)i, (float)(-i));
	}

	input[0] = new CsResource(sizeof(Data), dataSize, &dataA[0]);
	input[1] = new CsResource(sizeof(Data), dataSize, &dataB[0]);
	output = new CsResource(sizeof(Data), dataSize, NULL);

	ExecuteCs();
}

TestCompute::~TestCompute()
{
	for (UINT i = 0; i < 2; i++)
		SAFE_DELETE(input[i]);
	SAFE_DELETE(output);
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
	shader->AsShaderResource("Input1")->SetResource(input[0]->SRV());
	shader->AsShaderResource("Input2")->SetResource(input[1]->SRV());
	//shader->AsUAV("Output")->SetUnorderedAccessView(output->UAV());

	shader->Dispatch(0, 0, 1, 1, 1);

	FILE* file = fopen("Result.txt", "w");

	vector<Data> dataView(dataSize);
	output->Read(&dataView[0]);

	for (UINT i = 0; i < dataSize; i++)
	{
		fprintf(file, "V1 : (%f, %f, %f)", dataView[i].V1.x, dataView[i].V1.y, dataView[i].V1.z);
		fprintf(file, " - V2 : (%f, %f)\n", dataView[i].V2.x, dataView[i].V2.y);
	}

	fclose(file);
}
