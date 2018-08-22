#include "stdafx.h"
#include "Billboard.h"

Billboard::Billboard(ExecuteValues * values, wstring shaderFile, wstring diffuseFile)
	: GameModel(Materials + L"/Meshes/", L"Cube.material", Models + L"/Meshes/", L"Cube.mesh")
	, values(values)
{
	for (Material* material : model->Materials())
	{
		material->SetShader(shaderFile);
		material->SetDiffuseMap(diffuseFile);
	}
}

Billboard::~Billboard()
{
}

void Billboard::Update()
{	
	/*D3DXMATRIX R;
	values->MainCamera->Matrix(&R);
	
	D3DXMatrixInverse(&R, NULL, &R);

	float temp = -asinf(R._32);
	float x = cosf(temp);
	float y = R._33 / x;
	float z = R._22 / x;

	D3DXVECTOR3 vec;
	vec.x = acosf(y);
	vec.y = temp;
	vec.z = acosf(z);

	Rotate(D3DXVECTOR2(vec.x, vec.y));*/

	__super::Update();
}

void Billboard::Render()
{
	__super::Render();
}
