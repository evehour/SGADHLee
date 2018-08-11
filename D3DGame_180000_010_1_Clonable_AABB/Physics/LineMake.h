#pragma once

class LineMake
{
private:
	class ColorBuffer;
	typedef VertexColor VertexType;

public:
	LineMake();
	~LineMake();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

	void AddLine(D3DXVECTOR3 start, D3DXVECTOR3 end);
	void ClearBuffer();
	void UpdateBuffer();
	void SetPosition(D3DXVECTOR3& pos);
	void SetWorld(D3DXMATRIX& mat);

private:
	Shader * shader;
	WorldBuffer* worldBuffer;

	ID3D11Buffer* vertexBuffer = NULL;
	ID3D11Buffer* indexBuffer = NULL;

	UINT vertexCount, indexCount;

	VertexType* vertices = NULL;
	UINT* indices = NULL;

	ColorBuffer* colorBuffer;
	list<pair<D3DXVECTOR3, D3DXVECTOR3>> m_lineList;

private:
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
};