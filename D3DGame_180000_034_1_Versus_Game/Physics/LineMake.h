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

	void MeshData(vector<ModelVertexType> vertices, vector<UINT> indices, UINT objNum = 0);
	void MeshData(vector<VertexTextureNormal> vertices, vector<UINT> indices, UINT objNum = 0);
	void MeshData(vector<D3DXVECTOR3> vertices, vector<UINT> indices, UINT objNum = 0);

	void MakeAABB(const D3DXVECTOR3& minPos, const D3DXVECTOR3& maxPos);

	void AddLine(D3DXVECTOR3 start, D3DXVECTOR3 end);
	void SetColor(D3DXCOLOR& color);
	D3DXCOLOR GetColor() { return colorBuffer->Data.Color; }
	void UpdateBuffer();
	void InputBuffer(vector<VertexType> vertices, vector<UINT> indices);

	void ClearBuffer();

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

	D3DXMATRIX matWorld;

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