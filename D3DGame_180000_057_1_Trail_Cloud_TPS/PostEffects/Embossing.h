#pragma once

class Embossing : public Render2D
{
public:
	Embossing(ExecuteValues *values);
	~Embossing();

	void Render();

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Level = 1.0f;
		}

		struct Struct
		{
			float Level;
			int Width;
			int Height;

			float padding;
		} Data;
	};
	Buffer* buffer;
};