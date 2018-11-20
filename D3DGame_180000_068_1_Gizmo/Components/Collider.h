#pragma once
#include "Component.h"

class Collider : public Component
{
public:
	Collider();
	~Collider();

	virtual void Update();
	virtual void Render();

	D3DXVECTOR3 Position() { return this->position; }
	void Position(float x, float y, float z) { Position(D3DXVECTOR3(x, y, z)); }
	void Position(D3DXVECTOR3& val) { this->position = val; }

	D3DXVECTOR3 Rotation() { return this->rotation; }
	void Rotation(float x, float y, float z) { Rotation(D3DXVECTOR3(x, y, z)); }
	void Rotation(D3DXVECTOR3& val) { this->rotation = val; }

	D3DXVECTOR3 Scale() { return this->scale; }
	virtual void Scale(float x, float y, float z) { Scale(D3DXVECTOR3(x, y, z)); }
	virtual void Scale(D3DXVECTOR3& val) { this->scale = val; }

	D3DXMATRIX GetMatrix() { return this->matFinal; }
	virtual void UpdateData();

	void SetParentMatrix(D3DXMATRIX* matParent) { this->matParent = matParent; }

	int GetObjType() { return objType; }

	virtual bool IsCollision(Collider * other);

protected:
	virtual void CreateData() = 0;
	virtual void CreateBuffer();

protected:
	int objType;

	D3DXVECTOR3 up;
	D3DXVECTOR3 right;
	D3DXVECTOR3 forward;

	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 rotation;

	D3DXMATRIX matLocal, matFinal;
	D3DXMATRIX* matParent;

	Shader* shader;

	VertexColor* vertexData;
	UINT* indexData;

	UINT vertexCount, indexCount;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	WorldBuffer* worldBuffer;

	class LineMake* compass[3];

	class ColorBuffer : public ShaderBuffer
	{
	public:
		ColorBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Color = D3DXCOLOR(1, 1, 1, 1);
		}
		~ColorBuffer()
		{

		}

		struct Struct
		{
			D3DXCOLOR Color;
		} Data;
	};
	ColorBuffer* colorBuffer;
};