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
			Data.GammaLevel = 1;
		}

		struct Struct
		{
			float GammaLevel;

			int Width;
			int Height;
			float padding;
		} Data;
	};
	Buffer* buffer;
};