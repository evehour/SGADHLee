#pragma once

class Mosaic : public Render2D
{
public:
	Mosaic(ExecuteValues *values);
	~Mosaic();

	void Render();

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.MosaicLevel = 0;
		}

		struct Struct
		{
			float padding;

			int Width;
			int Height;
			float MosaicLevel;
		} Data;
	};
	Buffer* buffer;
};