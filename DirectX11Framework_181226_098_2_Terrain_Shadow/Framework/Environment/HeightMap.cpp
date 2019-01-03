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

	BYTE* temp = new BYTE[width * height * 4];
	r->Byte((void**)&temp, width * height * 4);

	r->Close();
	SAFE_DELETE(r);

	float* temp2 = reinterpret_cast<float*>(temp);
	for (UINT i = 0; i < (width * height); i++)
	{
		datas[i] = temp2[i] / 255.0f * maxHeight;
	}
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
	data.SysMemSlicePitch = width * height * sizeof(D3DXFLOAT16);

	HRESULT hr = D3D::GetDevice()->CreateTexture2D(&desc, &data, texture);
	assert(SUCCEEDED(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc; // 안에 포인터 있으면 0 초기화 불가능, 생성자가 없어도 안됨
	// union 그 구조체 안에 가장 큰 사이즈로 맞추고 나머지는 그 공간 공유해서 쓰는거
	// union에서도 padding 잡아줄 수 있음
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

void HeightMap::SaveFile2(wstring saveFile, ID3D11ShaderResourceView** HeightMap)
{
	ID3D11Texture2D* copyTempTexture;
	(*HeightMap)->GetResource((ID3D11Resource **)&copyTempTexture);

	D3D11_TEXTURE2D_DESC srcDesc;
	copyTempTexture->GetDesc(&srcDesc);


	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = srcDesc.Width; // 2049가 제대로 들어옴.
	desc.Height = srcDesc.Height; // 2049가 제대로 들어옴.
	desc.MipLevels = -1;
	desc.ArraySize = 1;
	desc.Format = srcDesc.Format;
	desc.SampleDesc = srcDesc.SampleDesc;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.MiscFlags = 0;

	HRESULT hr;

	ID3D11Texture2D* texture;
	hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &texture);
	assert(SUCCEEDED(hr));

	hr = D3DX11LoadTextureFromTexture(D3D::GetDC(), copyTempTexture, NULL, texture);
	assert(SUCCEEDED(hr));

	UINT dataSize = (2048) * (height);

	D3DXFLOAT16* _data16 = new D3DXFLOAT16[dataSize];
	float* _data32 = new float[dataSize]; 
	BYTE* _writeData = new BYTE[dataSize];

	// RowPitch가 4224이고, WORD라서 /2 하면 2112, 여기에 -62하면 2050값이 나온다.
	// 왜 RowPitch가 4098이 안나오는거지?

	D3D11_MAPPED_SUBRESOURCE map;
	D3D::GetDC()->Map(texture, 0, D3D11_MAP_READ, NULL, &map);
	{
		memcpy(_data16, map.pData, sizeof(D3DXFLOAT16) * dataSize);
	}
	D3D::GetDC()->Unmap(texture, 0);
	SAFE_RELEASE(texture);
	SAFE_RELEASE(copyTempTexture);

	vector<float> a;
	//Convert FLOAT16 to FLOAT32
	D3DXFloat16To32Array(_data32, _data16, dataSize);
	SAFE_DELETE_ARRAY(_data16);

	// Convert float to BYTE
	for (UINT i = 0; i < dataSize; i++)
	{
		//a.push_back(_data32[i]);
		_writeData[i] = (BYTE)(_data32[i] / maxHeight * 255.0f);
		//_writeData[i] = (BYTE)(datas[i] / maxHeight * 255.0f);
	}
	SAFE_DELETE_ARRAY(_data32);

	BinaryWriter* w = new BinaryWriter();
	w->Open(saveFile);

	w->Byte(_writeData, dataSize);

	w->Close();
	SAFE_DELETE(w);
	SAFE_DELETE_ARRAY(_writeData);
}
