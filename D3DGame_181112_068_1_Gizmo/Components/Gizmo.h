#pragma once
#include "Component.h"

class Gizmo : public Component
{
private:
	struct Quad;

public:
	enum GizmoAxis
	{
		X,
		Y,
		Z,
		XY,
		ZX,
		YZ,
		None
	};

	enum GizmoMode
	{
		Translate,
		Rotate,
		NonUniformScale,
		UniformScale
	};

	enum TransformSpace
	{
		Local,
		World
	};

	enum PivotType
	{
		ObjectCenter,
		SelectionCenter,
		WorldOrigin
	};

public:
	Gizmo(ExecuteValues* values);
	Gizmo(ExecuteValues* values, D3DXMATRIX world);
	~Gizmo();

	void Update();
	void Render();

public:
	bool Enable;

private:
	void Initialize();

private:
	ExecuteValues* values;
	bool _isActive;
	bool SelectionBoxesIsVisible;

	// -- Screen Scale -- //
	D3DXMATRIX _screenScaleMatrix;
	float _screenScale;

	// -- Position - Rotation -- //
	D3DXVECTOR3 _position;
	D3DXMATRIX _rotationMatrix;

	D3DXVECTOR3 _localForward;
	D3DXVECTOR3 _localUp;
	D3DXVECTOR3 _localRight;

	// -- Matrices -- //
	D3DXMATRIX _objectOrientedWorld;
	D3DXMATRIX _axisAlignedWorld;
	D3DXMATRIX _modelLocalSpace[3];

	// used for all drawing, assigned by local- or world-space matrices
	D3DXMATRIX _gizmoWorld;

	// the matrix used to apply to your whole scene, usually matrix.identity (default scale, origin on 0,0,0 etc.)
	D3DXMATRIX SceneWorld;

	// -- Lines (Vertices) -- //
	VertexColor* _translationLineVertices;
	const float LINE_LENGTH = 3.0f;
	const float LINE_OFFSET = 1.0f;

	// -- Quads -- //
	Quad* _quads[3];
	//private readonly BasicEffect _quadEffect;

	// -- Colors -- //
	D3DXCOLOR _axisColors[3];
	D3DXCOLOR _highlightColor;

	// -- UI Text -- //
	string _axisText[3];
	D3DXVECTOR3 _axisTextOffset;

	// -- Selection -- //
public:
	vector<class ITransformable*> Selection;

	// -- Modes & Selections -- //
	GizmoAxis ActiveAxis = GizmoAxis::None;
	GizmoMode ActiveMode = GizmoMode::Translate;
	TransformSpace ActiveSpace = TransformSpace::Local;
	PivotType ActivePivot = PivotType::SelectionCenter;

private:
	struct Quad
	{
		D3DXVECTOR3 Origin;
		D3DXVECTOR3 UpperLeft;
		D3DXVECTOR3 LowerLeft;
		D3DXVECTOR3 UpperRight;
		D3DXVECTOR3 LowerRight;
		D3DXVECTOR3 Normal;
		D3DXVECTOR3 Up;
		D3DXVECTOR3 Left;

		VertexTextureNormal Vertices[4];
		short Indexes[6];

		Quad(D3DXVECTOR3 origin, D3DXVECTOR3 normal, D3DXVECTOR3 up,
			float width, float height)
		{
			Origin = origin;
			Normal = normal;
			Up = up;

			// Calculate the quad corners
			D3DXVec3Cross(&Left, &normal, &Up);
			D3DXVECTOR3 uppercenter = (Up * height / 2) + origin;
			UpperLeft = uppercenter + (Left * width / 2);
			UpperRight = uppercenter - (Left * width / 2);
			LowerLeft = UpperLeft - (Up * height);
			LowerRight = UpperRight - (Up * height);

			FillVertices();
		}

		void FillVertices()
		{
			// Fill in texture coordinates to display full texture
			// on quad
			D3DXVECTOR2 textureUpperLeft = D3DXVECTOR2(0.0f, 0.0f);
			D3DXVECTOR2 textureUpperRight = D3DXVECTOR2(1.0f, 0.0f);
			D3DXVECTOR2 textureLowerLeft = D3DXVECTOR2(0.0f, 1.0f);
			D3DXVECTOR2 textureLowerRight = D3DXVECTOR2(1.0f, 1.0f);

			// Provide a normal for each vertex
			for (int i = 0; i < 4; i++)
			{
				Vertices[i].Normal = Normal;
			}

			// Set the position and texture coordinate for each
			// vertex
			Vertices[0].Position = LowerLeft;
			Vertices[0].Uv = textureLowerLeft;
			Vertices[1].Position = UpperLeft;
			Vertices[1].Uv = textureUpperLeft;
			Vertices[2].Position = LowerRight;
			Vertices[2].Uv = textureLowerRight;
			Vertices[3].Position = UpperRight;
			Vertices[3].Uv = textureUpperRight;

			// Set the index buffer for each vertex, using
			// clockwise winding
			Indexes[0] = 0;
			Indexes[1] = 1;
			Indexes[2] = 2;
			Indexes[3] = 2;
			Indexes[4] = 1;
			Indexes[5] = 3;
		}
	};
};