#include "stdafx.h"
#include "HeightMap.h"

#include "Utilities/BinaryFile.h"

HeightMap::HeightMap(UINT width, UINT height, float maxHeight)
	: width(width), height(height), maxHeight(maxHeight)
{
	datas = new float[width * height];
}

HeightMap::~HeightMap()
{
	SAFE_DELETE_ARRAY(datas);
}

void HeightMap::Load(wstring file)
{
	BYTE* tmp = new BYTE[width * height];

	BinaryReader* r = new BinaryReader();
	r->Open(file);
	{
		r->Byte((void**)&tmp, width * height);
	}
	r->Close();
	SAFE_DELETE(r);

	for (UINT i = 0; i < (width * height); i++)
		datas[i] = (float)(tmp[i]) / 255.0f * maxHeight;
}

void HeightMap::Data(UINT row, UINT col, float data)
{
	if (InBounds(row, col) == false)
		return;

	datas[row * height + col] = data;
}

float HeightMap::Data(UINT row, UINT col)
{
	if (InBounds(row, col) == false)
		return;

	return datas[row * height + col];
}

bool HeightMap::InBounds(UINT row, UINT col)
{
	return row >= 0 && row < height && col >= 0 && col < width;
}

void HeightMap::Smooth()
{
	float* dest = new float(height * width);
	for (UINT row = 0; row < height; row++)
	{
		for (UINT col = 0; col < width; col++)
		{
			dest[row * height + col] = Average(row, col);
		}
	}

	memcpy(datas, dest, sizeof(float) * width * height);
	SAFE_DELETE_ARRAY(dest);
}

float HeightMap::Average(UINT row, UINT col)
{
	float avg = 0.0f;
	float num = 0.0f;

	for (UINT m = row - 1; m <= row + 1; m++)
	{
		for (UINT n = col - 1; n <= col + 1; n++)
		{
			if (!InBounds(m, n)) continue;

			avg += datas[m * height + n];
			num++;
		}
	}
	return avg / num;
}
