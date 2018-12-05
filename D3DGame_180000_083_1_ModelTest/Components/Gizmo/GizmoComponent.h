#pragma once

#include "./Objects/GameRender.h"
#include "../../Draw/DebugLine.h"

class GizmoComponent
{
private:
	const float PRECISION_MODE_SCALE = 0.1f;
	const float scaleFactor = 25;


#pragma region Private Quad Struct
	struct Quad
	{
	public:
		D3DXVECTOR3 Origin;
		D3DXVECTOR3 UpperLeft;
		D3DXVECTOR3 LowerLeft;
		D3DXVECTOR3 UpperRight;
		D3DXVECTOR3 LowerRight;
		D3DXVECTOR3 Normal;
		D3DXVECTOR3 Up;
		D3DXVECTOR3 Left;

		VertexTextureNormal Vertices[4];
		UINT Indexes[6];
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
		UINT stride;
		UINT offset;

		void Setting(D3DXVECTOR3 origin, D3DXVECTOR3 normal, D3DXVECTOR3 up,
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

		Quad()
			: indexBuffer(NULL), vertexBuffer(NULL) {}

		~Quad()
		{
			SAFE_RELEASE(indexBuffer);
			SAFE_RELEASE(vertexBuffer);
		}

		void Render(Shader* shader)
		{
			D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
			D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			shader->DrawIndexed(0, 2, 6);
		}

	private:
		void FillVertices()
		{
			// Fill in texture coordinates to display full texture
			// on quad
			D3DXVECTOR2 textureUpperLeft  = D3DXVECTOR2(0.0f, 0.0f);
			D3DXVECTOR2 textureUpperRight = D3DXVECTOR2(1.0f, 0.0f);
			D3DXVECTOR2 textureLowerLeft  = D3DXVECTOR2(0.0f, 1.0f);
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

			stride = sizeof(VertexTextureNormal);
			offset = 0;

			//Create Line Vertex Buffer
			{
				D3D11_BUFFER_DESC desc = { 0 };
				desc.Usage = D3D11_USAGE_DYNAMIC;
				desc.ByteWidth = stride * 4;
				desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

				D3D11_SUBRESOURCE_DATA data = { 0 };
				data.pSysMem = Vertices;

				HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
				assert(SUCCEEDED(hr));
			}

			//Create Line Index Buffer
			{
				D3D11_BUFFER_DESC desc = { 0 };
				desc.Usage = D3D11_USAGE_DYNAMIC;
				desc.ByteWidth = sizeof(UINT) * 6;
				desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

				D3D11_SUBRESOURCE_DATA data = { 0 };
				data.pSysMem = Indexes;

				HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
				assert(SUCCEEDED(hr));
			}
		}
	};
#pragma endregion

public:
	enum class Axis { X, Y, Z, XY, ZX, YZ, None };
	enum class Mode { Translate, Rotate, NonUniformScale, UniformScale };
	enum class TransformSpace { Local, World };
	enum class PivotType { ObjectCenter, SelectionCenter, WorldOrigin };

	Axis ActiveAxis = Axis::None;
	Mode ActiveMode = Mode::Translate;
	TransformSpace ActiveSpace = TransformSpace::Local;
	PivotType ActivePivot = PivotType::SelectionCenter;

	// the matrix used to apply to your whole scene, usually matrix.identity (default scale, origin on 0,0,0 etc.)
	D3DXMATRIX SceneWorld;

public:
	GizmoComponent();
	~GizmoComponent();

	void Update();
	void Render();
	void SetEntites(vector<GameRender*> entities) { this->_selectionPool = entities; };
	void AddEntity(GameRender* entity);
	void SelectEntities(bool addToSelection, bool removeFromSelection);

private:

	struct Ray* ray;

	// -- Line Draw -- //
	Shader *lineShader;
	ID3D11Buffer* _translationLineVertexBuffer;
	
	bool _isActive;
	bool Enabled;
	bool SelectionBoxesIsVisible = false;

	// -- Matrices -- //
	D3DXMATRIX _objectOrientedWorld;
	D3DXMATRIX _axisAlignedWorld;
	D3DXMATRIX _modelLocalSpace[3];

	// used for all drawing, assigned by local- or world-space matrices
	D3DXMATRIX _gizmoWorld;

	// -- Gizmo Models -- //
	class GizmoModel* ActiveModels[3];

	// -- Lines (Vertices) -- //
	VertexColor _translationLineVertices[18];
	const float LINE_LENGTH = 3.0f;
	const float LINE_OFFSET = 1.0f;

	string _axisText[3];

	// -- Quads -- //
	Quad _quads[3];

	// -- Colors -- //
	D3DXCOLOR _axisColors[3];
	D3DXCOLOR _highlightColor;

	// -- BoundingBoxes -- //
	const float MULTI_AXIS_THICKNESS = 0.05f;
	const float SINGLE_AXIS_THICKNESS = 0.2f;

	// -- Selection -- //
	vector<GameRender*> _selectionPool;
	vector<GameRender*> Selection;

	D3DXVECTOR3 _translationDelta = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 _rotationVDelta;
	D3DXMATRIX _rotationDelta;
	D3DXVECTOR3 _scaleDelta = D3DXVECTOR3(0, 0, 0);


	// -- Translation Variables -- //
	D3DXVECTOR3 _tDelta;
	D3DXVECTOR3 _lastIntersectionPosition;
	D3DXVECTOR3 _intersectPosition;

	bool SnapEnabled = false;
	bool PrecisionModeEnabled;
	float TranslationSnapValue = 5;
	float RotationSnapValue = 30;
	float ScaleSnapValue = 0.5f;

	D3DXVECTOR3 _translationScaleSnapDelta;
	float _rotationSnapDelta;

	vector<VertexColor> _selectionBoxVertices;

	// -- Screen scale -- //
	float _screenScale;
	D3DXMATRIX _screenScaleMatrix;

	// -- Position - Rotation -- //
	D3DXVECTOR3 _position;
	D3DXMATRIX _rotationMatrix;

	D3DXVECTOR3 _localForward;
	D3DXVECTOR3 _localUp;
	D3DXVECTOR3 _localRight;

	struct BBox* xAxisBox;
	struct BBox* yAxisBox;
	struct BBox* zAxisBox;

	struct BBox* xzAxisBox;
	struct BBox* xyBox;
	struct BBox* yzBox;

	float radius;
	struct BSphere* xSphere;
	struct BSphere* ySphere;
	struct BSphere* zSphere;

protected:
	void PickObject(bool removeFromSelection);
	void ResetDeltas();

private:
	void Initialize();
	void SelectAxis();
	void SetGizmoPosition();
	D3DXVECTOR3 GetSelectionCenter();
	void ApplyColor(GizmoComponent::Axis axis, D3DXCOLOR color);
	void ApplyLineColor(UINT startIndex, UINT count, D3DXCOLOR color);
};