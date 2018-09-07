#pragma once

class GaussianBlur : public Render2D
{
public:
	GaussianBlur(ExecuteValues *values);
	~GaussianBlur();

	void Render();

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Select = 0;
			Data.Count = 0;
		}

		struct Struct
		{
			int Select;
			int Width;
			int Height;
			int Count;
		} Data;
	};
	Buffer* buffer;
};