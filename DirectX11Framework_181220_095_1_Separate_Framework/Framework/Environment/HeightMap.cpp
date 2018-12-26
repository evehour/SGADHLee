#include "Framework.h"
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
	BinaryReader* r = new BinaryReader();
	r->Open(file);

	BYTE* temp = new BYTE[width * height];
	r->Byte((void**)&temp, width * height);

	r->Close();
	SAFE_DELETE(r);

	for (UINT i = 0; i < (width * height); i++)
		datas[i] = (float)temp[i] / 255.0f * maxHeight;
		//datas[i] = 0;

	SAFE_DELETE_ARRAY(temp);
}

void HeightMap::Data(UINT row, UINT col, float data)
{
	if (InBounds(row, col) == false) 
		return;

	datas[row * height + col] = data;
}

float HeightMap::Data(UINT row, UINT col)
{
	if (!InBounds(row, col))
		return 0.0f;

	return datas[row * height + col];
}

bool HeightMap::InBounds(int row, int col)
{
	return row >= 0 && row < (int)height && col >= 0 && col < (int)width;
}

void HeightMap::Smooth()
{
	float* dest = new float[width * height];
	for (UINT row = 0; row < height; row++)
	{
		for (UINT col = 0; col < width; col++)
			dest[row * height + col] = Average(row, col);
	}

	memcpy(datas, dest, sizeof(float) * width * height);
	SAFE_DELETE_ARRAY(dest);
}

float HeightMap::Average(UINT row, UINT col)
{
	float average = 0.0f;
	float sum = 0.0f;

	for (UINT m = row - 1; m <= row + 1; m++)
	{
		for (UINT n = col - 1; n <= col + 1; n++)
		{
			if (InBounds(m, n) == false) continue;

			average += datas[m * height + n];
			sum++;
		}
	}

	return average / sum;
}

void HeightMap::BuildSRV(OUT ID3D11Texture2D** texture, OUT ID3D11ShaderResourceView ** srv, OUT ID3D11UnorderedAccessView ** uav)
{
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.Format = DXGI_FORMAT_R16_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;

	D3DXFLOAT16* temp = new D3DXFLOAT16[width * height];
	D3DXFloat32To16Array(temp, datas, width * height);

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = temp;
	data.SysMemPitch = width * sizeof(D3DXFLOAT16);

	HRESULT hr = D3D::GetDevice()->CreateTexture2D(&desc, &data, texture);
	assert(SUCCEEDED(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc; // �ȿ� ������ ������ 0 �ʱ�ȭ �Ұ���, �����ڰ� ��� �ȵ�
	// union �� ����ü �ȿ� ���� ū ������� ���߰� �������� �� ���� �����ؼ� ���°�
	// union������ padding ����� �� ����
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	hr = D3D::GetDevice()->CreateShaderResourceView(*texture, &srvDesc, srv);
	assert(SUCCEEDED(hr));


	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	uavDesc.Format = desc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	hr = D3D::GetDevice()->CreateUnorderedAccessView(*texture, &uavDesc, uav);
	assert(SUCCEEDED(hr));

	SAFE_DELETE_ARRAY(temp);
}
