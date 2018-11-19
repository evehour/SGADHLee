#include "stdafx.h"
#include "Gizmo.h"

Gizmo::Gizmo(ExecuteValues* values)
{
	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);

	Gizmo(values, mat);
}

Gizmo::Gizmo(ExecuteValues* values, D3DXMATRIX world)
	: values(values)
{
	SceneWorld = world;
	Enable = true;
	SelectionBoxesIsVisible = true;


}

Gizmo::~Gizmo()
{
}

void Gizmo::Update()
{
	D3DXVECTOR2 mousePosition = Mouse::Get()->GetPosition();

	if (_isActive)
	{
		//_lastIntersectionPosition = _intersectPosition;
	}
}

void Gizmo::Render()
{
}

void Gizmo::Initialize()
{
	_position = D3DXVECTOR3(0, 0, 0);
	_localForward = D3DXVECTOR3(0, 0, -1);
	_localUp = D3DXVECTOR3(0, 1, 0);
	_axisTextOffset = D3DXVECTOR3(0, 0.5f, 0);
	D3DXMatrixIdentity(&_rotationMatrix);
	D3DXMatrixIdentity(&_gizmoWorld);

	// -- Set local-space offset -- //
	_modelLocalSpace[0] = Math::CreateWorld(&D3DXVECTOR3(LINE_LENGTH, 0, 0), &D3DXVECTOR3(-1, 0, 0), &D3DXVECTOR3(0, 1, 0));
	_modelLocalSpace[1] = Math::CreateWorld(&D3DXVECTOR3(0, LINE_LENGTH, 0), &D3DXVECTOR3(0, -1, 0), &D3DXVECTOR3(-1, 0, 0));
	_modelLocalSpace[2] = Math::CreateWorld(&D3DXVECTOR3(0, 0, LINE_LENGTH), &D3DXVECTOR3(0, 0, -1), &D3DXVECTOR3(0, 1, 0));

	// -- Colors: X,Y,Z,Highlight -- //
	_axisColors[0] = D3DXCOLOR(1, 0, 0, 1);
	_axisColors[1] = D3DXCOLOR(0, 1, 0, 1);
	_axisColors[2] = D3DXCOLOR(0, 0, 1, 1);
	_highlightColor = D3DXCOLOR(1, 1, 0, 1);

#pragma region Translucent Quads
	const float halfLineOffset = LINE_OFFSET / 2;
	_quads[0] = new Quad(D3DXVECTOR3(halfLineOffset, halfLineOffset, 0), D3DXVECTOR3(0, 0, 1), D3DXVECTOR3(0, 1, 0), LINE_OFFSET, LINE_OFFSET); //XY
	_quads[1] = new Quad(D3DXVECTOR3(halfLineOffset, 0, halfLineOffset), D3DXVECTOR3(0, 1, 0), D3DXVECTOR3(1, 0, 0), LINE_OFFSET, LINE_OFFSET); //XZ
	_quads[2] = new Quad(D3DXVECTOR3(0, halfLineOffset, halfLineOffset), D3DXVECTOR3(1, 0, 0), D3DXVECTOR3(0, 1, 0), LINE_OFFSET, LINE_OFFSET); //ZY 
#pragma endregion

#pragma region Fill Axis - Line array
	vector<VertexColor> vertexList;

	// helper to apply colors
	D3DXCOLOR xColor = _axisColors[0];
	D3DXCOLOR yColor = _axisColors[1];
	D3DXCOLOR zColor = _axisColors[2];


	// -- X Axis -- // index 0 - 5
	vertexList.push_back(VertexColor(D3DXVECTOR3(halfLineOffset, 0, 0), xColor));
	vertexList.push_back(VertexColor(D3DXVECTOR3(LINE_LENGTH, 0, 0), xColor));

	vertexList.push_back(VertexColor(D3DXVECTOR3(LINE_OFFSET, 0, 0), xColor));
	vertexList.push_back(VertexColor(D3DXVECTOR3(LINE_OFFSET, LINE_OFFSET, 0), xColor));

	vertexList.push_back(VertexColor(D3DXVECTOR3(LINE_OFFSET, 0, 0), xColor));
	vertexList.push_back(VertexColor(D3DXVECTOR3(LINE_OFFSET, 0, LINE_OFFSET), xColor));

	// -- Y Axis -- // index 6 - 11
	vertexList.push_back(VertexColor(D3DXVECTOR3(0, halfLineOffset, 0), yColor));
	vertexList.push_back(VertexColor(D3DXVECTOR3(0, LINE_LENGTH, 0), yColor));

	vertexList.push_back(VertexColor(D3DXVECTOR3(0, LINE_OFFSET, 0), yColor));
	vertexList.push_back(VertexColor(D3DXVECTOR3(LINE_OFFSET, LINE_OFFSET, 0), yColor));

	vertexList.push_back(VertexColor(D3DXVECTOR3(0, LINE_OFFSET, 0), yColor));
	vertexList.push_back(VertexColor(D3DXVECTOR3(0, LINE_OFFSET, LINE_OFFSET), yColor));

	// -- Z Axis -- // index 12 - 17
	vertexList.push_back(VertexColor(D3DXVECTOR3(0, 0, halfLineOffset), zColor));
	vertexList.push_back(VertexColor(D3DXVECTOR3(0, 0, LINE_LENGTH), zColor));

	vertexList.push_back(VertexColor(D3DXVECTOR3(0, 0, LINE_OFFSET), zColor));
	vertexList.push_back(VertexColor(D3DXVECTOR3(LINE_OFFSET, 0, LINE_OFFSET), zColor));

	vertexList.push_back(VertexColor(D3DXVECTOR3(0, 0, LINE_OFFSET), zColor));
	vertexList.push_back(VertexColor(D3DXVECTOR3(0, LINE_OFFSET, LINE_OFFSET), zColor));

	_translationLineVertices = new VertexColor[vertexList.size()];
	UINT vertexCount = vertexList.size();
	copy
	(
		vertexList.begin(), vertexList.end(),
		stdext::checked_array_iterator<VertexColor *>(this->_translationLineVertices, vertexCount)
	);

#pragma endregion

}
