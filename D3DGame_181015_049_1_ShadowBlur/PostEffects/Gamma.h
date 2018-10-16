#pragma once

class Gamma : public Render2D
{
public:
	Gamma(ExecuteValues *values);
	~Gamma();

	void Render();

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.RGB = D3DXVECTOR3(1, 1, 1);
		}

		struct Struct
		{
			int Width;
			int Height;
			float padding[2];

			D3DXVECTOR3 RGB;
			float padding2;
		} Data;
	};
	Buffer* buffer;
};