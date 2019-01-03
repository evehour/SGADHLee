#pragma once

class TerrainRender
{
public:
	static const float MaxDistance;
	static const float MinDistance;
	static const float MaxTessellation;
	static const float MinTessellation;

public:
	TerrainRender(class Terrain* terrain);
	~TerrainRender();

	void Initialize();
	void Ready(Material* material);
	void Update();
	void Render(UINT pass = 0U);

private:
	void CalcAllPatchBoundsY();
	void CalcPatchBoundsY(UINT row, UINT col);
	void BuildQuadPatchVB();
	void BuildQuadPatchIB();
	//void BuildPatches();

	void CreateBlendMap();
	void SmoothBlendMap(vector<D3DXCOLOR>& colors);

private:
	D3DXMATRIX world;
	ID3D11Buffer* cBuffer;

	//bool bUseTessellation;
	class Terrain* terrain;

	ID3D11Buffer* quadPatchVB;
	ID3D11Buffer* quadPatchIB;

	vector<class Patch *> patches;

	TextureArray* layerMapArray;
	ID3D11ShaderResourceView* layerMapArraySRV;
	TextureArray* layerNormalMapArray;
	ID3D11ShaderResourceView* layerNormalMapArraySRV;
	ID3D11ShaderResourceView* blendMapSRV;

	ID3D11Texture2D* heightMapTexture;
	ID3D11ShaderResourceView* heightMapSRV;
	ID3D11UnorderedAccessView* heightMapUAV;

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

	class Frustum* frustum;

	bool bWireFrame;

private:
	// Using PixelPicking
	Render2D* render2D;
	RenderTargetView* renderTargetView;

public:
	ID3D11Texture2D* GetRenderTargetTexture() { return renderTargetView->Texture(); }
	ID3D11ShaderResourceView* GetRenderTargetSRV() { return renderTargetView->SRV(); }
	ID3D11Texture2D* GetHeightMapTexture() { return heightMapTexture; }
	ID3D11ShaderResourceView* GetHeightMapSRV() { return heightMapSRV; }
	ID3D11UnorderedAccessView* GetHeightMapUAV() { return heightMapUAV; }

private:
	D3DXVECTOR4 blendPositionRate;
	D3DXVECTOR2 blendMinMax[4];

	struct Buffer
	{
		D3DXCOLOR FogColor;

		float FogStart;
		float FogRange;

		float MinDistance;
		float MaxDistance;
		float MinTessellation;
		float MaxTessellation;

		float TexelCellSpaceU;
		float TexelCellSpaceV;
		float WorldCellSpace;

		D3DXVECTOR2 TexScale = D3DXVECTOR2(66.0f, 66.0f);
		float Padding;

		D3DXPLANE WorldFrustumPlanes[6];

	} buffer;
};