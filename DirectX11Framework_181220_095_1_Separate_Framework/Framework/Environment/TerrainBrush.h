#pragma once

class TerrainBrush
{
public:
	TerrainBrush(class Terrain* terrain, Shader* terrainShader, float heightMapWidth, float heightMapHeight);
	~TerrainBrush();

	void AddBrush(const wstring& name, const wstring& textureFile);
	void ChangeBrush(const UINT& idx);
	void ChangeBrush(const wstring& name);
	void ClearBrushes();

	void UpdatePosition(const D3DXVECTOR3& pixelPosition);

	void Brush(const bool& bUp, const float& rate);

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

	vector<wstring> brushNames;
	map<wstring, Texture*> brushes;
	
	Texture* brushImage;
	Render2D* render2DBrush;
	Render2D* render2DProjBrushOnTerrain;
	RenderTargetView* rtvBrush;
	D3DXVECTOR2 brushScale;

	// Projection Map
	ID3D11Texture2D* pickCopyMap;
	ID3D11ShaderResourceView* pickCopySRV;
};