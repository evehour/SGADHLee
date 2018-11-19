#pragma once

#include "./Objects/GameRender.h"

#include "../../Draw/DebugLine.h"

class Gizmo : public GameRender
{
private:
	const float PRECISION_MODE_SCALE = 0.1f;

public:
	enum class Axis { X, Y, Z, XY, ZX, YZ, None };
	enum class Mode { Translate, Rotate, NonUniformScale, UniformScale };
	enum class TransformSpace { Local, World };
	enum class PivotType { ObjectCenter, SelectionCenter, WorldOrigin };

	Axis ActiveAxis = Axis::None;
	Mode ActiveMode = Mode::Translate;
	TransformSpace ActiveSpace = TransformSpace::Local;
	PivotType ActivePivot = PivotType::SelectionCenter;

	bool SnapEnabled = false;
	bool PrecisionModeEnabled;
	float TranslationSnapValue = 5;
	float RotationSnapValue = 30;
	float ScaleSnapValue = 0.5f;

	D3DXMATRIX SceneWorld;

public:
	Gizmo(ExecuteValues* values);
	~Gizmo();

	void Set(class GameRender* render);
	
	void Update();
	void UpdateLine();
	void Render();
	void SetEntites(vector<GameRender*> entities) { this->_selectionPool = entities; };
	void SelectEntities(bool addToSelection, bool removeFromSelection);

private:
	ExecuteValues* values;

	struct Ray* ray;
	// -- Selection -- //
	vector<GameRender*> _selectionPool;
	vector<GameRender*> Selection;

	D3DXVECTOR3 _translationDelta = D3DXVECTOR3(0, 0, 0);
	D3DXMATRIX _rotationDelta;
	D3DXVECTOR3 _scaleDelta = D3DXVECTOR3(0, 0, 0);


	// -- Translation Variables -- //
	D3DXVECTOR3 _tDelta;
	D3DXVECTOR3 _lastIntersectionPosition;
	D3DXVECTOR3 _intersectPosition;

	// -- Position - Rotation -- //
	D3DXMATRIX _rotationMatrix;

	D3DXVECTOR3 _translationScaleSnapDelta;
	float _rotationSnapDelta;


	
	D3DXCOLOR axisColor[3];
	D3DXCOLOR highlightColor;
	
	struct BBox* xAxisBox;
	struct BBox* yAxisBox;
	struct BBox* zAxisBox;

	float lineOffset;
	float halfLineOffset;
	float lineLength;
	float singleAxisThickness; 

	vector<class ILine*> axiss;
	DebugLine* axisLine;

	DepthStencilState* depthMode[2];

	class GameRender* render;

	DebugLine* axis[3];

protected:
	void PickObject(bool removeFromSelection);
	void ResetDeltas();
};