#pragma once

class TerrainBrush
{
public:
	enum class BrushType : UINT
	{
		Normal = 0, Flat, Smooth,
		MAX
	};

public:
	TerrainBrush(class Terrain* terrain, float heightMapWidth, float heightMapHeight);
	~TerrainBrush();

	void Initialize();
	void Ready();
	void Render();

	void SaveHeightMap(wstring saveFile);
	Shader* GetBrushShader() { return brushShader; }

public:
	void SetBrush(Texture* const t);
	Texture* GetBrushTexture() { return brushImage; }

	void UpdatePosition(const D3DXVECTOR3& pixelPosition);

	D3DXVECTOR2 Scale() { return brushScale; }
	void Scale(const D3DXVECTOR2& scale);
	void Power(float val);
	float Power() { return brushPower; }

	void Brush(TerrainBrush::BrushType brushType, const bool& bUp = true);

	bool Hovering() { return bHovering; }
	void Hovering(const bool& val);

	void TestRender();

private:
	void SettingBrush();
	void SettingBrushMap();


private:
	class Terrain* terrain;
	Shader* terrainShader;
	Shader* brushShader;

	float heightMapWidth, heightMapHeight;

	// Test
	Render2D* testViewRender2D;

	// Brush
	bool bHovering;
	
	Texture* brushImage;
	Render2D* render2DBrush;
	Render2D* render2DProjBrushOnTerrain;
	RenderTargetView* rtvBrush;

	D3DXVECTOR2 pickPixelPosition;
	D3DXVECTOR2 brushScale;
	float brushPower;
	bool bBrushUp;
	UINT groupW, groupH;

	// Projection Map
	ID3D11Texture2D* pickCopyMap;
	ID3D11ShaderResourceView* pickCopySRV;
};