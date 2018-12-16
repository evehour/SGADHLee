#include "stdafx.h"
#include "TerrainRender.h"
#include "Patch.h"
#include "Terrain.h"
#include "HeightMap.h"

const float TerrainRender::MaxDistance = 500.0f;
const float TerrainRender::MinDistance = 20.0f;
const float TerrainRender::MaxTessellation = 6.0f;
const float TerrainRender::MinTessellation = 0.0f;

TerrainRender::TerrainRender(Material * material, Terrain * terrain)
	: material(material), terrain(terrain)
{
	D3DXMatrixIdentity(&world);
}

TerrainRender::~TerrainRender()
{
	for (Patch* patch : patches)
		SAFE_DELETE(patch);

	SAFE_RELEASE(quadPatchIB);
	SAFE_RELEASE(quadPatchVB);

	SAFE_RELEASE(bvhIndexBuffer);
	SAFE_RELEASE(bvhVertexBuffer);

	SAFE_DELETE(layerMapArray);
	SAFE_RELEASE(layerMapArraySRV);
}

void TerrainRender::Initialize()
{
	patchVertexRows = ((terrain->Desc().HeightMapHeight - 1) / Terrain::CellsPerPatch) + 1;
	patchVertexCols = ((terrain->Desc().HeightMapWidth - 1) / Terrain::CellsPerPatch) + 1;

	patchVerticesCount = patchVertexRows * patchVertexCols;
	patchQuadFacesCount = (patchVertexRows - 1) * (patchVertexCols - 1);

	CalcAllPatchBoundsY();

	BuildQuadPatchVB();
	BuildQuadPatchIB();

	CreateBlendMap();

	vector<wstring> textures;
	for (UINT i = 0; i < 5; i++)
		textures.push_back(terrain->Desc().LayerMapFile[i]);
	layerMapArray = new TextureArray(textures, 512, 512);
	layerMapArraySRV = layerMapArray->SRV();
}

void TerrainRender::Render()
{
	UINT stride = sizeof(TerrainCP);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &quadPatchVB, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(quadPatchIB, DXGI_FORMAT_R16_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	material->GetShader()->AsMatrix("World")->SetMatrix(world);
	material->GetShader()->DrawIndexed(0, 0, patchQuadFacesCount * 4);
}

void TerrainRender::CalcAllPatchBoundsY()
{
	patchBoundsY.assign(patchQuadFacesCount, D3DXVECTOR2(0, 0));

	for (UINT row = 0; row < patchVertexRows - 1; row++)
	{
		for (UINT col = 0; col < patchVertexCols - 1; col++)
		{
			CalcPatchBoundsY(row, col);
		}
	}
}

void TerrainRender::CalcPatchBoundsY(UINT row, UINT col)
{
	UINT x0 = col * terrain->CellsPerPatch;
	UINT x1 = (col + 1) * terrain->CellsPerPatch;

	UINT y0 = row * terrain->CellsPerPatch;
	UINT y1 = (row + 1) * terrain->CellsPerPatch;

	float minY = FLT_MAX;
	float maxY = FLT_MIN;

	for (UINT y = y0; y <= y1; y++)
	{
		for (UINT x = x0; x <= x1; x++)
		{
			minY = min(minY, terrain->Data(y, x));
			maxY = max(maxY, terrain->Data(y, x));
		}
	}
	UINT patchID = row * (patchVertexCols - 1) + col;
	patchBoundsY[patchID] = D3DXVECTOR2(minY, maxY);
}

void TerrainRender::BuildQuadPatchVB()
{
	TerrainCP* patchVertices = new TerrainCP[patchVerticesCount];
	float halfWidth = 0.5f * (float)terrain->Width();
	float halfDepth = 0.5f * (float)terrain->Depth();

	float patchWidth = (float)terrain->Width() / (float)(patchVertexCols - 1);
	float patchDepth = (float)terrain->Depth() / (float)(patchVertexRows - 1);

	float du = 1.0f / (float)(patchVertexCols - 1);
	float dv = 1.0f / (float)(patchVertexRows - 1);

	for (UINT row = 0; row < patchVertexRows; row++)
	{
		float z = halfDepth - (float)row * patchDepth;
		for (UINT col = 0; col < patchVertexCols; col++)
		{
			float x = -halfWidth + (float)col * patchWidth;
			UINT vertId = row * patchVertexCols + col;

			patchVertices[vertId].Position = D3DXVECTOR3(x, 0, z);
			patchVertices[vertId].Uv = D3DXVECTOR2(col * du, row * dv);
		}
	}

	for (UINT row = 0; row < patchVertexRows - 1; row++)
	{
		for (UINT col = 0; col < patchVertexCols - 1; col++)
		{
			UINT patchID = row * (patchVertexCols - 1) + col;
			UINT vertID = row * patchVertexCols + col;
			patchVertices[vertID].BoundsY = patchBoundsY[patchID];
		}
	}

	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = sizeof(TerrainCP) * patchVerticesCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = &patchVertices[0];

	HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &quadPatchVB);
	assert(SUCCEEDED(hr));

	SAFE_DELETE_ARRAY(patchVertices);
}

void TerrainRender::BuildQuadPatchIB()
{
	vector<WORD> indices;
	for (UINT row = 0; row < patchVertexRows - 1; row++)
	{
		for (UINT col = 0; col < patchVertexCols - 1; col++)
		{
			indices.push_back(row * patchVertexCols + col);
			indices.push_back(row * patchVertexCols + col + 1);
			indices.push_back((row + 1) * patchVertexCols + col);
			indices.push_back((row + 1) * patchVertexCols + col + 1);
		}
	}

	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = sizeof(WORD) * indices.size();
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = &indices[0];

	HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &quadPatchIB);
	assert(SUCCEEDED(hr));
}

void TerrainRender::CreateBlendMap()
{
	vector<D3DXCOLOR> colors;
	HeightMap* heightMap = terrain->GetHeightMap();
	float maxHeightMap = heightMap->MaxHeight();

	for (UINT y = 0; y < heightMap->Height(); y++)
	{
		for (UINT x = 0; x < heightMap->Width(); x++)
		{
			float elevation = terrain->Data(y, x);
			D3DXCOLOR color;
			if (elevation > maxHeightMap * (0.05f + Math::Random(-0.05f, 0.05f)));
			{
				// dark green grass texture
				color.r = (elevation / maxHeightMap) + Math::Random(-0.05f, 0.05f);
			}
			if (elevation > maxHeightMap * (0.4f + Math::Random(-0.15f, 0.15f)))
			{
				color.g = elevation / maxHeightMap + Math::Random(-0.05f, 0.05f);
			}
			if (elevation > maxHeightMap * (0.75f + Math::Random(-0.1f, 0.1f)))
			{
				color.a = elevation / maxHeightMap + Math::Random(-0.05f, 0.05f);
			}

			colors.push_back(color);
		}
	}

	SmoothBlendMap(colors);
	SmoothBlendMap(colors);

	vector<UINT> colors8b(colors.size());
	const float f = 255.0f;

	vector<D3DXCOLOR>::iterator it = colors.begin();
	for (UINT i = 0; it != colors.end(); it++, i++)
		colors8b[i] = ((UINT)((f * it->a)) << 24)
		+ ((UINT)((f * it->b)) << 16)
		+ ((UINT)((f * it->g)) << 8)
		+ ((UINT)((f * it->r)) << 0);

	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

	ID3D11Texture2D* texture = 0;
	//   Create Blend Texture2D
	{
		UINT _width = terrain->GetHeightMap()->Width();
		UINT _height = terrain->GetHeightMap()->Height();

		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = _width;
		desc.Height = _height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &colors8b[0];
		data.SysMemPitch = _width * sizeof(UINT);
		data.SysMemSlicePitch = _width * _height * sizeof(UINT);

		HRESULT hr = D3D::GetDevice()->CreateTexture2D(&desc, &data, &texture);
		assert(SUCCEEDED(hr));
	}

	//   Create Shader Resource View (To . blendMapSRV)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		desc.Format = format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MostDetailedMip = 0;
		desc.Texture2D.MipLevels = 1;

		HRESULT hr = D3D::GetDevice()->CreateShaderResourceView(texture, &desc, &blendMapSRV);
		assert(SUCCEEDED(hr));
	}

	SAFE_RELEASE(texture);
	colors.clear();
	colors8b.clear();
}

void TerrainRender::SmoothBlendMap(vector<D3DXCOLOR>& colors)
{
	for (UINT y = 0; y < terrain->GetHeightMap()->Height(); y++)
	{
		for (UINT x = 0; x < terrain->GetHeightMap()->Width(); x++)
		{
			D3DXCOLOR sum = colors[x + y * terrain->GetHeightMap()->Height()];
			UINT num = 0;
			for (UINT y1 = y - 1; y1 < y + 2; y1++)
			{
				for (UINT x1 = x - 1; x1 > x + 1; x1++)
				{
					if (!terrain->GetHeightMap()->InBounds(y1, x1))
						continue;
					sum += colors[x1 + y1 * terrain->GetHeightMap()->Height()];
					num++;
				}
			}
			colors[x + y + terrain->GetHeightMap()->Height()] = sum / (float)num;
		}
	}
}
