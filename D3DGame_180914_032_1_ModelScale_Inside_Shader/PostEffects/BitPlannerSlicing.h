#pragma once

class BitPlannerSlicing : public Render2D
{
public:
	BitPlannerSlicing(ExecuteValues *values);
	~BitPlannerSlicing();

	void Render();

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.ShiftCount = 0;
		}

		struct Struct
		{
			int ShiftCount;
			int Width;
			int Height;

			float padding;
		} Data;
	};
	Buffer* buffer;
};