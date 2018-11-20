#pragma once

class TrailRender
{
public:
	TrailRender(int splitCount, wstring textureName);
	~TrailRender();

	void Update(D3DXMATRIX parent);
	void Render();

	void ClearTrail(D3DXMATRIX mat);

private:
	bool bViewTrail;
	RasterizerState* rasterizerState[2];
	BlendState* blendState[2];

	int boneIdx;
	int splitCount;
	ID3D11Buffer* vertexBuffer;

	float deltaStoreTime;

	void SetAndShiftTrailMatBuffer(D3DXMATRIX& insertFirstMat);

	class TrailBuffer : public ShaderBuffer
	{
	public:
		TrailBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			for (UINT i = 0; i < 128; i++)
				D3DXMatrixIdentity(&Data.buffer[i]);
		}

		struct Struct
		{
			D3DXMATRIX buffer[128];

			int TrailCount;
			float Padding[3];
		} Data;
	};

	TrailBuffer* trailBuffer;
	Texture* trailTexture;
	Shader* trailShader;

};