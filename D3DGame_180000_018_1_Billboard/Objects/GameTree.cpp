#include "stdafx.h"
#include "GameTree.h"

GameTree::GameTree(ExecuteValues * values, wstring shaderFile, wstring diffuseFile)
	: GameModel(Materials + L"/Meshes/", L"Quad.material", Models + L"/Meshes/", L"Quad.mesh")
	, values(values)
{
	cullMode[0] = new RasterizerState();
	cullMode[1] = new RasterizerState();
	cullMode[1]->FrontCounterClockwise(true);

	//Create Tree
	{
		D3DXMATRIX R;

		GameModel* _tree = new Tree(values, shaderFile, diffuseFile);
		tree.push_back(_tree);

#if false
		_tree = new Tree(values, shaderFile, diffuseFile);
		D3DXMatrixRotationY(&R, Math::ToRadian(270.0f));
		_tree->RootAxis(R);
		tree.push_back(_tree);

		_tree = new Tree(values, shaderFile, diffuseFile);
		D3DXMatrixRotationY(&R, Math::ToRadian(90.0f));
		_tree->RootAxis(R);
		tree.push_back(_tree);
#else
		_tree = new Tree(values, shaderFile, diffuseFile);
		((Tree *)_tree)->SetTreeAxis(0, Math::ToRadian(360 - 45), 0);
		tree.push_back(_tree);

		_tree = new Tree(values, shaderFile, diffuseFile);
		((Tree *)_tree)->SetTreeAxis(0, Math::ToRadian(45), 0);
		tree.push_back(_tree);
#endif
	}
	
}

GameTree::~GameTree()
{
	for (RasterizerState* r : cullMode)
		SAFE_DELETE(r);

	for (GameModel* gm : tree)
		SAFE_DELETE(gm);
}

void GameTree::Update()
{
	__super::Update();

	for (GameModel* gm : tree)
	{
		gm->Position(Position());
		gm->Scale(Scale());
		gm->Update();
	}
}

void GameTree::Render()
{
	cullMode[1]->RSSetState();
	for (GameModel* gm : tree)
	{
		gm->Render();
	}
	cullMode[0]->RSSetState();
}



GameTree::Tree::Tree(ExecuteValues * values, wstring shaderFile, wstring diffuseFile)
	: GameModel(Materials + L"/Meshes/", L"Quad.material", Models + L"/Meshes/", L"Quad.mesh")
	, values(values), treeAxis(0, 0, 0)
{
	for (Material* material : model->Materials())
	{
		material->SetShader(shaderFile);
		material->SetDiffuseMap(diffuseFile);
	}
}

GameTree::Tree::~Tree()
{
}

void GameTree::Tree::Update()
{
	D3DXMATRIX R;
	values->MainCamera->Matrix(&R);

	D3DXMatrixInverse(&R, NULL, &R);

	float x = asin(-R._32);
	float y = atan2(R._31, R._33);
	float z = atan2(R._12, R._22);

	Rotation(0 + treeAxis.x, y + treeAxis.y, 0 + treeAxis.z);
	__super::Update();
}

void GameTree::Tree::Render()
{
	__super::Render();
}
