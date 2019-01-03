#pragma once
#include "Systems/IExecute.h"

class TestTerrain : public IExecute
{
public:
	enum class EditorMode
	{
		TerrainBrush = 0,
		ObjectSpot,
		PlantSpot,
		Config,
		Max,
	};
public:
	// IExecute을(를) 통해 상속됨
	void Initialize() override;
	void Ready() override;
	void Destroy() override;
	void Update() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void ResizeScreen() override;

private:
	void UITerrainBrush();
	void UIObjectBrush();
	void UIPlantBrush();

private:
	class CubeSky* sky;
	class Terrain* terrain;
	Material* terrainMaterial;
	class PathFinding* pathFinding;
	class  TerrainBrush* brush;

	class Grass* grass;

private:
	D3DXVECTOR3 MousePosition;
	bool bUsingPathFinding;
	bool picked;
	D3DXVECTOR3 pickPixel;
	D3DXVECTOR3 pickWorldPosition;
	D3DXVECTOR3 pickMapTileWorldPos;
	
private:
	EditorMode editorMode;
	Texture* typeIcon[4];

private:
	Render2D* testViewRender2D;

	// Brush
	Texture* brushImage;
	Render2D* render2DBrush;
	Render2D* render2DProjBrushOnTerrain;
	RenderTargetView* rtvBrush;
	Shader* brushShader;

	UINT selectedBrush;

	// Projection Map
	ID3D11Texture2D* pickCopyMap;
	ID3D11ShaderResourceView* pickCopySRV;

	// Plant
	float plantWindLimitFactor;
	float plantSensitive;
	D3DXVECTOR2 plantScale;
	int plantType;
	int plantShakeType;

	int plantInsertCount;

	vector<string> plantTextureFilenames;
	vector<Texture *> plantTextures;

	Texture* emptyTexture;

private:
	// Using Save
	vector<wstring> plantTextureFiles;

private:
	bool bBrushHovering;

	typedef pair<wstring, Texture*> BrushPair;
	map<int, BrushPair> brushes;
	int currentBrushShape;
};