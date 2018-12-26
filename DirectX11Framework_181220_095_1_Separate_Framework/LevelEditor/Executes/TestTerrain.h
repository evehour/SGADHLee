#pragma once
#include "Systems/IExecute.h"

class TestTerrain : public IExecute
{
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
	class CubeSky* sky;
	class Terrain* terrain;
	class PathFinding* pathFinding;
	class  TerrainBrush* brush;

private:
	bool bUsingPathFinding;
	bool picked;
	D3DXVECTOR3 pickPixel;
	D3DXVECTOR3 pickWorldPosition;
	D3DXVECTOR3 pickMapTileWorldPos;

private:
	Render2D* testViewRender2D;

	// Brush
	Texture* brushImage;
	Render2D* render2DBrush;
	Render2D* render2DProjBrushOnTerrain;
	RenderTargetView* rtvBrush;
	Shader* brushShader;

	// Projection Map
	ID3D11Texture2D* pickCopyMap;
	ID3D11ShaderResourceView* pickCopySRV;
};