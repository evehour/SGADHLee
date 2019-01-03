#include "Framework.h"
#include "TerrainRender.h"

#include "Terrain.h"
#include "Patch.h"
#include "HeightMap.h"
#include "./Viewer/Frustum.h"

const float TerrainRender::MaxDistance = 500.0f;
const float TerrainRender::MinDistance = 20.0f;
const float TerrainRender::MaxTessellation = 6.0f;
const float TerrainRender::MinTessellation = 0.0f;

TerrainRender::TerrainRender(Terrain * terrain)
	: terrain(terrain)
	, bvhVertexBuffer(NULL), bvhIndexBuffer(NULL), blendMapSRV(NULL)
	, heightMapTexture(NULL), heightMapSRV(NULL), heightMapUAV(NULL)
{
}

TerrainRender::~TerrainRender()
{
	SAFE_DELETE(render2D);
	SAFE_DELETE(renderTargetView);

	SAFE_RELEASE(cBuffer);

	for (Patch* patch : patches)
		SAFE_DELETE(patch);

	SAFE_RELEASE(quadPatchVB);
	SAFE_RELEASE(quadPatchIB);

	SAFE_RELEASE(bvhVertexBuffer);
	SAFE_RELEASE(bvhIndexBuffer);

	SAFE_DELETE(layerMapArray);
	SAFE_DELETE(layerNormalMapArray);

	SAFE_RELEASE(blendMapSRV);

	SAFE_RELEASE(heightMapUAV);
	SAFE_RELEASE(heightMapSRV);
	SAFE_RELEASE(heightMapTexture);

	SAFE_DELETE(frustum);

}

void TerrainRender::Initialize()
{
	D3DXMatrixIdentity(&world);
	frustum = new Frustum(500);

	patchVertexRows = (UINT)(((terrain->Desc().HeightMapHeight - 1) / Terrain::CellsPerPatch) + 1);
	patchVertexCols = (UINT)(((terrain->Desc().HeightMapWidth - 1) / Terrain::CellsPerPatch) + 1);

	patchVerticesCount = patchVertexRows * patchVertexCols;
	patchQuadFacesCount = (patchVertexRows - 1) * (patchVertexCols - 1);

	//if (bUseTessellation == true)
	//{
	CalcAllPatchBoundsY();
	//}
	//else
	//{
	//	BuildPatches();
	//}

#ifdef __USE_BLENDMAP__
	CreateBlendMap();
#endif

	buffer.FogStart = 300.0f;//15.0f;
	buffer.FogRange = 200.0f;//175.0f;
	buffer.FogColor = D3DXCOLOR(1, 1, 1, 1);
	buffer.MinDistance = MinDistance;
	buffer.MaxDistance = MaxDistance;
	buffer.MinTessellation = MinTessellation;
	buffer.MaxTessellation = MaxTessellation;
	buffer.TexelCellSpaceU = 1.0f / terrain->Desc().HeightMapWidth;
	buffer.TexelCellSpaceV = 1.0f / terrain->Desc().HeightMapHeight;
	//buffer.TexScale = D3DXVECTOR2(2.5f, 2.5f);
	buffer.WorldCellSpace = terrain->Desc().CellSpacing;

	blendPositionRate = { 0.06f, 0.40f, 0.0f, 0.75f };
	blendMinMax[0] = { 0.05f, 0.05f };
	blendMinMax[1] = { 0.15f, 0.15f };
	blendMinMax[2] = { 0.01f, 0.01f };
	blendMinMax[3] = { 0.05f, 0.05f };
}

void TerrainRender::Ready(Material* material)
{
	this->material = material;

	vector<wstring> textures, texturesNormal;
	for (int i = 0; i < 5; i++)
	{
		textures.push_back(terrain->Desc().layerMapFile[i]);
		texturesNormal.push_back(terrain->Desc().layerNormalMapFile[i]);
	}
	layerMapArray = new TextureArray(textures, 512, 512);
	layerNormalMapArray = new TextureArray(texturesNormal, 512, 512);
	layerMapArraySRV = layerMapArray->SRV();
	layerNormalMapArraySRV = layerNormalMapArray->SRV();

	terrain->GetHeightMap()->BuildSRV(&heightMapTexture, &heightMapSRV, &heightMapUAV);

	Shader* shader = material->GetShader();

#ifdef __USE_BLENDMAP__
	shader->AsShaderResource("BlendMap")->SetResource(blendMapSRV);
#endif

	D3DDesc d3dDesc;
	D3D::GetDesc(&d3dDesc);
	renderTargetView = new RenderTargetView((UINT)d3dDesc.Width, (UINT)d3dDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT);

	render2D = new Render2D();
	render2D->Scale(400, 400);
	render2D->Position(500, 500);
	render2D->SRV(renderTargetView->SRV());

	BuildQuadPatchVB();
	BuildQuadPatchIB();

	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(Buffer);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &cBuffer);
	assert(SUCCEEDED(hr));

#ifndef __USE_BLENDMAP__
	shader->AsVector("BlendPositionRate")->SetFloatVector(blendPositionRate);
	shader->AsScalar("MaxHeight")->SetFloat(terrain->Desc().HeightScale);
#endif

	shader->AsVector("HeightMapSize")->SetFloatVector(D3DXVECTOR2(terrain->Desc().HeightMapHeight - 1.0f, terrain->Desc().HeightMapWidth - 1.0f));
	shader->AsVector("TerrainSize")->SetFloatVector(D3DXVECTOR2(terrain->Width(), terrain->Depth()));
}

void TerrainRender::Update()
{
	frustum->Update();
}

void TerrainRender::Render(UINT pass)
{
	frustum->GetPlanes(buffer.WorldFrustumPlanes);

	D3D11_MAPPED_SUBRESOURCE subResource;
	HRESULT hr = D3D::GetDC()->Map
	(
		cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
	);
	assert(SUCCEEDED(hr));

	memcpy(subResource.pData, &buffer, sizeof(Buffer));
	D3D::GetDC()->Unmap(cBuffer, 0);

	hr = material->GetShader()->AsConstantBuffer("CB_Terrain")->SetConstantBuffer(cBuffer);
	assert(SUCCEEDED(hr));


	UINT stride = sizeof(TerrainCP);
	UINT offset = 0;

	ID3D11RenderTargetView* rtvs[2];
	ID3D11DepthStencilView* dsv;
	
	if (pass == (UINT)-1)
	{
		pass = 4;
	}
	else
	{
		renderTargetView->Clear();
		rtvs[0] = D3D::Get()->GetRenderTargetView();
		rtvs[1] = renderTargetView->RTV();
		dsv = D3D::Get()->GetDepthStencilView();

		D3D::Get()->SetRenderTargets(2, rtvs, dsv);
		material->GetShader()->AsShaderResource("ShadowMap")->SetResource(Shadowmap->DSRV());
		material->GetShader()->AsShaderResource("LayerMapArray")->SetResource(layerMapArraySRV);
		material->GetShader()->AsShaderResource("LayerNormalMapArray")->SetResource(layerNormalMapArraySRV);
	}
	material->GetShader()->AsShaderResource("HeightMap")->SetResource(heightMapSRV);


	D3D::GetDC()->IASetVertexBuffers(0, 1, &quadPatchVB, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(quadPatchIB, DXGI_FORMAT_R16_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	//int pass = bWireFrame == true ? 1 : 0;

	material->GetShader()->AsMatrix("World")->SetMatrix(world);
	material->GetShader()->DrawIndexed(0, pass, patchQuadFacesCount * 4);

	material->GetShader()->AsShaderResource("ShadowMap")->SetResource(nullptr);
	material->GetShader()->AsShaderResource("HeightMap")->SetResource(nullptr);
	material->GetShader()->AsShaderResource("LayerMapArray")->SetResource(nullptr);
	material->GetShader()->AsShaderResource("LayerNormalMapArray")->SetResource(nullptr);

	if (pass != 4)
	{
		rtvs[1] = NULL;
		D3D::GetDC()->OMSetRenderTargets(2, rtvs, dsv);
	}
	material->GetShader()->Apply(0, pass);

	//render2D->Render();
	//D3D::Get()->SetRenderTarget();
}

void TerrainRender::CalcAllPatchBoundsY()
{
	patchBoundsY.assign(patchQuadFacesCount, D3DXVECTOR2(0, 0));

	for (UINT row = 0; row < patchVertexRows - 1; row++)
		for (UINT col = 0; col < patchVertexCols - 1; col++)
			CalcPatchBoundsY(row, col);
}

void TerrainRender::CalcPatchBoundsY(UINT row, UINT col)
{
	UINT x0 = col * Terrain::CellsPerPatch;
	UINT x1 = (col + 1) * Terrain::CellsPerPatch;

	UINT y0 = row * Terrain::CellsPerPatch;
	UINT y1 = (row + 1) * Terrain::CellsPerPatch;

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
	vector<TerrainCP> vert;
	vert.assign(patchVerticesCount, TerrainCP());
	float halfWidth = 0.5f * terrain->Width();
	float halfDepth = 0.5f * terrain->Depth();

	float patchWidth = terrain->Width() / (float)(patchVertexCols - 1);
	float patchDepth = terrain->Depth() / (float)(patchVertexRows - 1);

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
			vert[vertId] = patchVertices[vertId];
		}
	}

	for (UINT row = 0; row < patchVertexRows - 1; row++)
	{
		for (UINT col = 0; col < patchVertexCols - 1; col++)
		{
			UINT patchID = row * (patchVertexCols - 1) + col;
			UINT vertID = row * patchVertexCols + col;
			patchVertices[vertID].BoundsY = patchBoundsY[patchID];
			vert[vertID].BoundsY = patchBoundsY[patchID];
		}
	}

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = sizeof(TerrainCP) * patchVerticesCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = &patchVertices[0];

	HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &quadPatchVB);
	assert(SUCCEEDED(hr));

	SAFE_DELETE_ARRAY(patchVertices);
}

void TerrainRender::BuildQuadPatchIB()
{
	vector<WORD> indices;
	for (WORD row = 0; row < (WORD)patchVertexRows - 1; row++)
	{
		for (WORD col = 0; col < (WORD)patchVertexCols - 1; col++)
		{
			indices.push_back(row * (WORD)patchVertexCols + col);
			indices.push_back(row * (WORD)patchVertexCols + col + 1);
			indices.push_back((row + 1) * (WORD)patchVertexCols + col);
			indices.push_back((row + 1) * (WORD)patchVertexCols + col + 1);
		}
	}

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = sizeof(WORD) * indices.size();
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = &indices[0];

	HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &quadPatchIB);
	assert(SUCCEEDED(hr));
}

void TerrainRender::CreateBlendMap()
{
	HeightMap* heightMap = terrain->GetHeightMap();

	vector<D3DXCOLOR> colors;

	UINT height = heightMap->Height();
	UINT width = heightMap->Width();
	float maxHeight = heightMap->MaxHeight();

	for (UINT y = 0; y < height; y++)
	{
		for (UINT x = 0; x < width; x++)
		{
			float elevation = heightMap->Data(y, x);

			D3DXCOLOR color = D3DXCOLOR(0, 0, 0, 0);

			if (elevation > maxHeight * (0.06f + Math::Random(-0.05f, 0.05f)))
			{
				// dark green grass texture
				color.r = elevation / maxHeight + Math::Random(-0.05f, 0.05f);
			}
			if (elevation > maxHeight * (0.04f + Math::Random(-0.15f, 0.15f)))
			{
				// stone texture
				color.g = elevation / maxHeight + Math::Random(-0.05f, 0.05f);
			}
			if (elevation > maxHeight * (0.75f + Math::Random(-0.1f, 0.1f)))
			{
				// snow texture
				color.a = elevation / maxHeight + Math::Random(-0.05f, 0.05f);
			}

			colors.push_back(color);
		}
	}

	SmoothBlendMap(colors);
	SmoothBlendMap(colors);

	DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT;

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
		data.pSysMem = &colors[0];
		data.SysMemPitch = _width * sizeof(D3DXCOLOR);

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
}

void TerrainRender::SmoothBlendMap(vector<D3DXCOLOR>& colors)
{
	HeightMap* heightMap = terrain->GetHeightMap();

	int height = (int)heightMap->Height();
	int width = (int)heightMap->Width();
	float maxHeight = heightMap->MaxHeight();

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			D3DXCOLOR sum = colors[x + y * height];
			UINT num = 1;
			for (int y1 = y - 1; y1 < y + 2; y1++)
			{
				for (int x1 = x - 1; x1 < x + 1; x1++)
				{
					if (heightMap->InBounds(y1, x1) == false)
						continue;

					sum += colors[x1 + y1 * height];
					num++;
				} // for(x1)
			} // for(y1)

			colors[x + y * height] = sum / (float)num;
		} // for(x)
	} // for(y)
}
