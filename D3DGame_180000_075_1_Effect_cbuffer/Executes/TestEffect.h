#pragma once
#include "Execute.h"

class TestEffect : public Execute
{
public:
	TestEffect();
	~TestEffect();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {};

private:
	Shader *shader1, *shader2;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	Vertex* vertices;
	UINT* indices;

	struct BBox* box;
	class CBuffer* cbuffer;

	struct _Buffer
	{
		D3DXVECTOR4 PlusPos;
		D3DXVECTOR3 PlusColor;
		float padding;
	} _buffer;
};