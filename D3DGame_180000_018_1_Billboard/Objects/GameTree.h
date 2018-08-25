#pragma once
#include "GameModel.h"

class GameTree : public GameModel
{
public:
	GameTree(ExecuteValues* values, wstring shaderFile, wstring diffuseFile);
	~GameTree();

	void Update();
	void Render();

private:

	class Tree : public GameModel
	{
	public:
		Tree(ExecuteValues* values, wstring shaderFile, wstring diffuseFile);
		~Tree();

		void Update();
		void Render();

		void SetTreeAxis(D3DXVECTOR3& args) { treeAxis = args; }
		void SetTreeAxis(float x, float y, float z) {
			treeAxis.x = x;
			treeAxis.y = y;
			treeAxis.z = z;
		}
	private:
		ExecuteValues* values;
		D3DXVECTOR3 treeAxis;
	};

	ExecuteValues* values;

	RasterizerState* cullMode[2];

	vector<GameModel *> tree;
};