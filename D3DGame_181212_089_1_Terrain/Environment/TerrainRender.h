#pragma once

class TerrainRender
{
public:
	TerrainRender(Material* material, class Terrain* terrain);
	~TerrainRender();

	void Initialize();

	void Render();

public:
	static const float MaxDistance;
	static const float MinDistance;
	static const float MaxTessellation;
	static const float MinTessellation;

private:
	void CalcAllPatchBoundsY();
	void CalcPatchBoundsY(UINT row, UINT col);
	void BuildQuadPatchVB();
	void BuildQuadPatchIB();

	void CreateBlendMap();
	void SmoothBlendMap(vector<D3DXCOLOR>& colors);

private:
	D3DXMATRIX world;

	class Terrain* terrain;

	ID3D11Buffer* quadPatchVB;
	ID3D11Buffer* quadPatchIB;

	vector<class Patch *> patches;

	TextureArray* layerMapArray;
	ID3D11ShaderResourceView* layerMapArraySRV;
	ID3D11ShaderResourceView* blendMapSRV;
	ID3D11ShaderResourceView* heightMapSRV;

	UINT patchVerticesCount;
	UINT patchQuadFacesCount;
	UINT patchVertexRows;
	UINT patchVertexCols;

	Material* material;
	vector<D3DXVECTOR2> patchBoundsY;

	vector<VertexColor> bvhVertices;
	vector<WORD> bvhIndices;

	ID3D11Buffer* bvhVertexBuffer;
	ID3D11Buffer* bvhIndexBuffer;

	UINT aabbCount;
};