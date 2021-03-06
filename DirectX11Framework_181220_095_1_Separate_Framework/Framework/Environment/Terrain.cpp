#include "Framework.h"
#include "Terrain.h"

#include "HeightMap.h"
#include "TerrainRender.h"

const UINT Terrain::CellsPerPatch = 64;

Terrain::Terrain(InitDesc & desc)
	: desc(desc)
{
	render = new TerrainRender(this->desc.material, this);
	
	heightMap = new HeightMap((UINT)(this->desc.HeightMapWidth), (UINT)(this->desc.HeightMapHeight), this->desc.HeightScale);
	heightMap->Load(this->desc.HeightMap);

	render->Initialize();
}

Terrain::~Terrain()
{
	SAFE_DELETE(render);
	SAFE_DELETE(heightMap);
	SAFE_DELETE(desc.material);
}

void Terrain::Update()
{

}

void Terrain::Render()
{
	render->Render();
}

void Terrain::Data(UINT row, UINT col, float data)
{
	heightMap->Data(row, col, data);
}

float Terrain::Data(UINT row, UINT col)
{
	return heightMap->Data(row, col);
}

float Terrain::Width()
{
	return (desc.HeightMapWidth - 1) * desc.CellSpacing;
}

float Terrain::Depth()
{
	return (desc.HeightMapHeight - 1) * desc.CellSpacing;
}

float Terrain::GetHeight(float x, float z)
{
	float c = (x + 0.5f * Width()) / desc.CellSpacing;
	float d = (z - 0.5f * Depth()) / -desc.CellSpacing;
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	float h00 = heightMap->Data(row, col);
	float h01 = heightMap->Data(row, col + 1);
	float h10 = heightMap->Data((row + 1), col);
	float h11 = heightMap->Data((row + 1), col + 1);

	float s = c - col;
	float t = d - row;

	float uy = 0.0f, vy = 0.0f;

	if (s + t <= 1.0f)
	{
		uy = h01 - h00;
		vy = h01 - h11;
	}
	else {
		uy = h10 - h11;
		vy = h01 - h11;
	}

	return h11 + (1.0f - s) * uy + (1.0f - t) * vy;
}

bool Terrain::Picking(OUT D3DXVECTOR3 * pickPixel, OUT D3DXVECTOR3 * pickWorldPos)
{
	float x, z;
	D3DXVECTOR3 position = Mouse::Get()->GetPosition();
	D3DXVECTOR4 result = Texture::ReadPixel128(render->GetRenderTargetTexture(), (UINT)position.x, (UINT)position.y);
	x = result.x;
	z = result.y;

	if (pickPixel != NULL)
	{
		pickPixel->x = x * (desc.HeightMapWidth - 1.0f);
		pickPixel->z = z * (desc.HeightMapHeight - 1.0f);
	}

	if (pickWorldPos != NULL)
	{
		pickWorldPos->x = x * Width() - (Width() * 0.5f);
		pickWorldPos->z = z * Depth() - (Depth() * 0.5f);
	}

	return x + z > 0;
}

ID3D11Texture2D * Terrain::GetHeightMapTexture()
{
	if (render)
		return render->GetHeightMapTexture();
	else
		return nullptr;
}

ID3D11ShaderResourceView * Terrain::GetHeightMapSRV()
{
	if (render)
		return render->GetHeightMapSRV();
	else
		return nullptr;
}

ID3D11UnorderedAccessView * Terrain::GetHeightMapUAV()
{
	if (render)
		return render->GetHeightMapUAV();
	else
		return nullptr;
}
