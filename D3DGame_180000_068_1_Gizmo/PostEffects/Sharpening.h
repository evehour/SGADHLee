#pragma once

class Sharpening : public Render2D
{
public:
	Sharpening(ExecuteValues *values);
	~Sharpening();

	void Render();

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Level = 0;
		}

		struct Struct
		{
			int Level;
			int Width;
			int Height;

			float padding;
		} Data;
	};
	Buffer* buffer;
};