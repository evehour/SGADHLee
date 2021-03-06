#pragma once
#include "GameRender.h"

class GameModel : public GameRender
{
public:
	GameModel
	(
		wstring matFolder, wstring matFile
		, wstring meshFolder, wstring meshFile
	);
	virtual ~GameModel();

	void Velocity(D3DXVECTOR3& vec);
	D3DXVECTOR3 Velocity();

	virtual void Update();
	virtual void Render();

	void RotateDegree(D3DXVECTOR2 amount);
	void Rotate(D3DXVECTOR2 amount);

	void Scale(D3DXVECTOR3 amount);

	Model* GetModel() { return model; }

protected:
	void CalcPosition();

protected:
	Model* model;
	Shader* shader;

	D3DXVECTOR3 velocity;

	D3DXMATRIX matRotation;
	D3DXMATRIX matScale;

	vector<D3DXMATRIX> boneTransforms;

private:
	class RenderBuffer : public ShaderBuffer
	{
	public:
		RenderBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Index = 0;
		}

		struct Struct
		{
			int Index;

			float Padding[3];
		} Data;
	};

	RenderBuffer* renderBuffer;
};