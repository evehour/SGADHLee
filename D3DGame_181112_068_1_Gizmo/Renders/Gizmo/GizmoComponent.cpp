#include "stdafx.h"
#include "GizmoComponent.h"
#include "GizmoModel.h"

#include "./Boundings/Ray.h"
#include "./Boundings/BBox.h"
#include "./Boundings/BSphere.h"

GizmoComponent::GizmoComponent(ExecuteValues* values)
	: values(values)
	, _isActive(true), SelectionBoxesIsVisible(false), PrecisionModeEnabled(false)
	, _position(0, 0, 0), _localForward(0, 0, 1), _localUp(0, 1, 0)
{
	xAxisBox = new BBox(
		D3DXVECTOR3(LINE_OFFSET, 0, 0),
		D3DXVECTOR3(LINE_OFFSET + LINE_LENGTH, SINGLE_AXIS_THICKNESS, SINGLE_AXIS_THICKNESS));
	yAxisBox = new BBox(
		D3DXVECTOR3(0, LINE_OFFSET, 0),
		D3DXVECTOR3(SINGLE_AXIS_THICKNESS, LINE_OFFSET + LINE_LENGTH, SINGLE_AXIS_THICKNESS));
	zAxisBox = new BBox(
		D3DXVECTOR3(0, 0, LINE_OFFSET),
		D3DXVECTOR3(SINGLE_AXIS_THICKNESS, SINGLE_AXIS_THICKNESS, LINE_OFFSET + LINE_LENGTH));

	xzAxisBox = new BBox(
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(LINE_OFFSET, MULTI_AXIS_THICKNESS, LINE_OFFSET));
	xyBox = new BBox(
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(LINE_OFFSET, LINE_OFFSET, MULTI_AXIS_THICKNESS));
	yzBox = new BBox(
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(MULTI_AXIS_THICKNESS, LINE_OFFSET, LINE_OFFSET));

	xSphere = new BSphere();
	ySphere = new BSphere();
	zSphere = new BSphere();

	_axisColors[(int)Axis::X] = D3DXCOLOR(1, 0, 0, 1);
	_axisColors[(int)Axis::Y] = D3DXCOLOR(0, 1, 0, 1);
	_axisColors[(int)Axis::Z] = D3DXCOLOR(0, 0, 1, 1);

	Enabled = true;

	depthMode[0] = new DepthStencilState();
	depthMode[1] = new DepthStencilState();
	depthMode[1]->DepthEnable(false);

	rasterizerState[0] = new RasterizerState();
	rasterizerState[1] = new RasterizerState();
	rasterizerState[1]->CullMode(D3D11_CULL_NONE);

	worldBuffer = new WorldBuffer();
	lineShader = new Shader(Shaders + L"003_Color_Line.hlsl");

	D3DXMatrixIdentity(&_rotationMatrix);
	D3DXMatrixIdentity(&_rotationDelta);
	D3DXMatrixIdentity(&_gizmoWorld);
	D3DXMatrixIdentity(&SceneWorld);

	Initialize();
}

void GizmoComponent::Initialize()
{
	// -- Set local-space offset -- //
	_modelLocalSpace[0] = Math::CreateWorld(&D3DXVECTOR3(LINE_LENGTH, 0, 0), &D3DXVECTOR3(1, 0, 0), &D3DXVECTOR3(0, 1, 0));
	_modelLocalSpace[1] = Math::CreateWorld(&D3DXVECTOR3(0, LINE_LENGTH, 0), &D3DXVECTOR3(0, 1, 0), &D3DXVECTOR3(-1, 0, 0));
	_modelLocalSpace[2] = Math::CreateWorld(&D3DXVECTOR3(0, 0, LINE_LENGTH), &D3DXVECTOR3(0, 0, 1), &D3DXVECTOR3(0, 1, 0));

	// -- Colors: X,Y,Z,Highlight -- //
	_axisColors[0] = D3DXCOLOR(1, 0, 0, 1);
	_axisColors[1] = D3DXCOLOR(0, 1, 0, 1);
	_axisColors[2] = D3DXCOLOR(0, 0, 1, 1);
	_highlightColor = D3DXCOLOR(1, 215.0f / 255.0f, 0, 1);

	// text projected in 3D
	_axisText[0] = "X";
	_axisText[1] = "Y";
	_axisText[2] = "Z";

	#pragma region Translucent Quads
	const float halfLineOffset = LINE_OFFSET / 2;

	_quads[0].Setting(D3DXVECTOR3(halfLineOffset, halfLineOffset, 0), D3DXVECTOR3(0, 0, 1), D3DXVECTOR3(0, 1, 0), LINE_OFFSET,
		LINE_OFFSET); //XY
	_quads[1].Setting(D3DXVECTOR3(halfLineOffset, 0, halfLineOffset), D3DXVECTOR3(0, 1, 0), D3DXVECTOR3(1, 0, 0), LINE_OFFSET,
		LINE_OFFSET); //XZ
	_quads[2].Setting(D3DXVECTOR3(0, halfLineOffset, halfLineOffset), D3DXVECTOR3(1, 0, 0), D3DXVECTOR3(0, 1, 0), LINE_OFFSET,
		LINE_OFFSET); //ZY 
	#pragma endregion

	#pragma region Axis Line
	D3DXCOLOR xColor = _axisColors[0];
	D3DXCOLOR yColor = _axisColors[1];
	D3DXCOLOR zColor = _axisColors[2];

	// -- X Axis -- // index 0 - 5
	_translationLineVertices[0] = { D3DXVECTOR3(halfLineOffset, 0, 0) , xColor };
	_translationLineVertices[1] = { D3DXVECTOR3(LINE_LENGTH, 0, 0) , xColor };

	_translationLineVertices[2] = { D3DXVECTOR3(LINE_OFFSET, 0, 0) , xColor };
	_translationLineVertices[3] = { D3DXVECTOR3(LINE_OFFSET, LINE_OFFSET, 0) , xColor };

	_translationLineVertices[4] = { D3DXVECTOR3(LINE_OFFSET, 0, 0) , xColor };
	_translationLineVertices[5] = { D3DXVECTOR3(LINE_OFFSET, 0, LINE_OFFSET) , xColor };

	// -- Y Axis -- // index 6 - 11
	_translationLineVertices[6] = { D3DXVECTOR3(0, halfLineOffset, 0) , yColor };
	_translationLineVertices[7] = { D3DXVECTOR3(0, LINE_LENGTH, 0) , yColor };

	_translationLineVertices[8] = { D3DXVECTOR3(0, LINE_OFFSET, 0) , yColor };
	_translationLineVertices[9] = { D3DXVECTOR3(LINE_OFFSET, LINE_OFFSET, 0) , yColor };

	_translationLineVertices[10] = { D3DXVECTOR3(0, LINE_OFFSET, 0) , yColor };
	_translationLineVertices[11] = { D3DXVECTOR3(0, LINE_OFFSET, LINE_OFFSET) , yColor };

	// -- Z Axis -- // index 12 - 17
	_translationLineVertices[12] = { D3DXVECTOR3(0, 0, halfLineOffset) , zColor };
	_translationLineVertices[13] = { D3DXVECTOR3(0, 0, LINE_LENGTH) , zColor };

	_translationLineVertices[14] = { D3DXVECTOR3(0, 0, LINE_OFFSET) , zColor };
	_translationLineVertices[15] = { D3DXVECTOR3(LINE_OFFSET, 0, LINE_OFFSET) , zColor };

	_translationLineVertices[16] = { D3DXVECTOR3(0, 0, LINE_OFFSET) , zColor };
	_translationLineVertices[17] = { D3DXVECTOR3(0, LINE_OFFSET, LINE_OFFSET) , zColor };

	//Create Line Vertex Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(VertexColor) * 18;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = _translationLineVertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &_translationLineVertexBuffer);
		assert(SUCCEEDED(hr));
	}
	#pragma endregion

#pragma region Gizmo Models
	ActiveModels[0] = new GizmoModel(GizmoModel::Kind::Translate);
	ActiveModels[1] = new GizmoModel(GizmoModel::Kind::Rotate);
	ActiveModels[2] = new GizmoModel(GizmoModel::Kind::Scale);
#pragma endregion

	radius = 1.0f;
}

GizmoComponent::~GizmoComponent()
{
	SAFE_RELEASE(_translationLineVertexBuffer);
	SAFE_DELETE(lineShader);
	SAFE_DELETE(worldBuffer);

	SAFE_DELETE(xAxisBox);
	SAFE_DELETE(yAxisBox);
	SAFE_DELETE(zAxisBox);
	SAFE_DELETE(xzAxisBox);
	SAFE_DELETE(xyBox);
	SAFE_DELETE(yzBox);
	SAFE_DELETE(xSphere);
	SAFE_DELETE(ySphere);
	SAFE_DELETE(zSphere);

	for (UINT i = 0; i < 2; i++)
	{
		SAFE_DELETE(depthMode[i]);;
		SAFE_DELETE(rasterizerState[i]);
	}

	for (UINT i = 0; i < 3; i++)
	{
		SAFE_DELETE(ActiveModels[i]);
	}
}

void GizmoComponent::Update()
{
	// select entities with your cursor (add the desired keys for add-to / remove-from -selection)
	if (Mouse::Get()->Down(0))
	{
		SelectEntities(Keyboard::Get()->Press(VK_LCONTROL), Keyboard::Get()->Press(VK_LMENU));
	}

	// set the active mode like translate or rotate
	if (Keyboard::Get()->Down('1'))
		ActiveMode = Mode::Translate;
	else if (Keyboard::Get()->Down('2'))
		ActiveMode = Mode::Rotate;
	else if (Keyboard::Get()->Down('3'))
		ActiveMode = Mode::NonUniformScale;
	else if (Keyboard::Get()->Down('4'))
		ActiveMode = Mode::UniformScale;

	// toggle precision mode
	if (Keyboard::Get()->Down(VK_LSHIFT))
		PrecisionModeEnabled = !PrecisionModeEnabled;

	if (_isActive)
	{
		_lastIntersectionPosition = _intersectPosition;

		if (Mouse::Get()->Press(0) && ActiveAxis != Axis::None)
		{
			D3DXVECTOR3 tPosition = _position;
			D3DXMATRIX invMat;
			D3DXMatrixInverse(&invMat, NULL, &_rotationMatrix);
			D3DXVec3TransformCoord(&tPosition, &tPosition, &invMat);


			D3DXVECTOR3 delta = D3DXVECTOR3(0, 0, 0);

			Ray ray;
			D3DXVECTOR3 camPos;
			values->MainCamera->Position(&camPos);
			//ray.SettingRay(camPos, values->MainCamera->Direction(values->Viewport, values->Perspective), invMat);
			ray.SettingRay(camPos, values->MainCamera->Direction(values->Viewport, values->Perspective));

			D3DXPLANE plane;
			D3DXVECTOR3 vIntersectionPosition;
			D3DXVECTOR3* vIsIntersection;

			switch (ActiveMode)
			{
				case Mode::UniformScale:
				case Mode::NonUniformScale:
				case Mode::Translate:
					{
						#pragma region Translate & Scale
						switch (ActiveAxis)
						{
							case Axis::XY:
							case Axis::X:
								{
									D3DXVECTOR3 vForward = D3DXVECTOR3(0, 0, 1);
									plane = D3DXPLANE(vForward.x, vForward.y, vForward.z, tPosition.z);

									vIsIntersection = D3DXPlaneIntersectLine(&vIntersectionPosition, &plane, &ray.Position, &ray.Direction);

									if (vIsIntersection != NULL)
									{
										_intersectPosition = vIntersectionPosition;
										if (_lastIntersectionPosition != D3DXVECTOR3(0, 0, 0))
										{
											_tDelta = _intersectPosition - _lastIntersectionPosition;
										}
										delta = ActiveAxis == Axis::X
											? D3DXVECTOR3(_tDelta.x, 0, 0)
											: D3DXVECTOR3(_tDelta.x, _tDelta.y, 0);
									}
								}
								break;
							case Axis::Z:
							case Axis::YZ:
							case Axis::Y:
								{
									plane = D3DXPLANE(-1, 0, 0, tPosition.x);
									vIsIntersection = D3DXPlaneIntersectLine(&vIntersectionPosition, &plane, &ray.Position, &ray.Direction);

									if (vIsIntersection != NULL)
									{
										_intersectPosition = vIntersectionPosition;
										if (_lastIntersectionPosition != D3DXVECTOR3(0, 0, 0))
										{
											_tDelta = _intersectPosition - _lastIntersectionPosition;
										}
										switch (ActiveAxis)
										{
											case Axis::Y:
												delta = D3DXVECTOR3(0, _tDelta.y, 0);
												break;
											case Axis::Z:
												delta = D3DXVECTOR3(0, 0, _tDelta.z);
												break;
											default:
												delta = D3DXVECTOR3(0, _tDelta.y, _tDelta.z);
												break;
										}
									}
								}
								break;
							case Axis::ZX:
								{
									plane = D3DXPLANE(0, -1, 0, tPosition.y);
									vIsIntersection = D3DXPlaneIntersectLine(&vIntersectionPosition, &plane, &ray.Position, &ray.Direction);

									if (vIsIntersection != NULL)
									{
										_intersectPosition = vIntersectionPosition;
										if (_lastIntersectionPosition != D3DXVECTOR3(0, 0, 0))
										{
											_tDelta = _intersectPosition - _lastIntersectionPosition;
										}
									}
									delta = D3DXVECTOR3(_tDelta.x, 0, _tDelta.z);
								}
								break;
						}


						if (SnapEnabled)
						{
							float snapValue = TranslationSnapValue;
							if (ActiveMode == Mode::UniformScale || ActiveMode == Mode::NonUniformScale)
								snapValue = ScaleSnapValue;
							if (PrecisionModeEnabled)
							{
								delta *= PRECISION_MODE_SCALE;
								snapValue *= PRECISION_MODE_SCALE;
							}

							_translationScaleSnapDelta += delta;

							delta = D3DXVECTOR3(
								(int)(_translationScaleSnapDelta.x / snapValue) * snapValue,
								(int)(_translationScaleSnapDelta.y / snapValue) * snapValue,
								(int)(_translationScaleSnapDelta.z / snapValue) * snapValue);

							_translationScaleSnapDelta -= delta;
						}
						else if (PrecisionModeEnabled)
							delta *= PRECISION_MODE_SCALE;


						if (ActiveMode == Mode::Translate)
						{
							// transform (local or world)
							D3DXVec3TransformCoord(&delta, &delta, &_rotationMatrix);
							_translationDelta = delta * 5.0f;
						}
						else if (ActiveMode == Mode::NonUniformScale || ActiveMode == Mode::UniformScale)
						{
							// -- Apply Scale -- //
							_scaleDelta += delta;
						}
						#pragma endregion
					}
					break;

				case Mode::Rotate:
					{
						#pragma region Rotate
						float delta = Mouse::Get()->GetMoveValue().x;

						if (SnapEnabled)
						{
							float snapValue = Math::ToRadian(RotationSnapValue);
							if (PrecisionModeEnabled)
							{
								delta *= PRECISION_MODE_SCALE;
								snapValue *= PRECISION_MODE_SCALE;
							}

							_rotationSnapDelta += delta;

							float snapped = (int)(_rotationSnapDelta / snapValue) * snapValue;
							_rotationSnapDelta -= snapped;

							delta = snapped;
						}
						else if (PrecisionModeEnabled)
							delta *= PRECISION_MODE_SCALE;

						// rotation matrix to transform - if more than one objects selected, always use world-space.
						D3DXMATRIX rot;
						D3DXMatrixIdentity(&rot);
						rot._11 = SceneWorld._11;
						rot._12 = SceneWorld._12;
						rot._13 = SceneWorld._13;
						rot._21 = SceneWorld._21;
						rot._22 = SceneWorld._22;
						rot._23 = SceneWorld._23;
						rot._31 = SceneWorld._31;
						rot._32 = SceneWorld._32;
						rot._33 = SceneWorld._33;

						D3DXMATRIX deltaRot;
						switch (ActiveAxis)
						{
							case Axis::X:
								D3DXMatrixRotationAxis(&deltaRot, &D3DXVECTOR3(_rotationMatrix._11, _rotationMatrix._12, _rotationMatrix._13), delta);
								rot *= deltaRot;
								_rotationVDelta = D3DXVECTOR3(delta, 0, 0);
								break;
							case Axis::Y:
								D3DXMatrixRotationAxis(&deltaRot, &D3DXVECTOR3(_rotationMatrix._21, _rotationMatrix._22, _rotationMatrix._23), delta);
								rot *= deltaRot;
								_rotationVDelta = D3DXVECTOR3(0, delta, 0);
								break;
							case Axis::Z:
								D3DXMatrixRotationAxis(&deltaRot, &D3DXVECTOR3(_rotationMatrix._31, _rotationMatrix._32, _rotationMatrix._33), delta);
								rot *= deltaRot;
								_rotationVDelta = D3DXVECTOR3(0, 0, delta);
								break;
						}
						_rotationDelta = rot;

						#pragma endregion
					}
					break;
			}
		}
		else
		{
			if (!Mouse::Get()->Press(0) && !Mouse::Get()->Press(1))
			{
				SelectAxis();
			}
		}

		SetGizmoPosition();

		// -- Trigger Translation, Rotation & Scale events -- //
		if (Mouse::Get()->Press(0))
		{
			D3DXMATRIX IdentityMatrix;
			D3DXMatrixIdentity(&IdentityMatrix);
			if (_translationDelta != D3DXVECTOR3(0, 0, 0))
			{
				for (GameRender* entity : Selection)
				{
					D3DXVECTOR3 ePos = entity->Position();
					ePos += _translationDelta;
					entity->Position(ePos);
				}

				_translationDelta = D3DXVECTOR3(0, 0, 0);
			}
			if (_rotationDelta != IdentityMatrix)
			{
				for (GameRender* entity : Selection)
				{
					D3DXVECTOR3 eRot = entity->RotationDegree();
					eRot += _rotationVDelta;
					entity->RotationDegree(eRot);
				}

				_rotationDelta = IdentityMatrix;
			}
			if (_scaleDelta != D3DXVECTOR3(0, 0, 0))
			{
				for (GameRender* entity : Selection)
				{
					D3DXVECTOR3 eScale = entity->Scale();
					eScale += _scaleDelta;
					entity->Scale(eScale);
				}
				_scaleDelta = D3DXVECTOR3(0, 0, 0);
			}
		}
	}

	if (Selection.size() < 1)
	{
		_isActive = false;
		ActiveAxis = Axis::None;
		return;
	}

	// helps solve visual lag (1-frame-lag) after selecting a new entity
	if (!_isActive)
		SetGizmoPosition();

	_isActive = true;

	// -- Scale Gizmo to fit on-screen -- //
	D3DXVECTOR3 camPos;
	values->MainCamera->Position(&camPos);

	D3DXVECTOR3 vLength = camPos - _position;
	_screenScale = D3DXVec3Length(&vLength) / scaleFactor;
	D3DXMatrixScaling(&_screenScaleMatrix, _screenScale, _screenScale, _screenScale);

	_localForward = Selection[0]->Direction();
	_localUp = Selection[0]->Up();
	// -- Vector Rotation (Local/World) -- //
	D3DXVec3Normalize(&_localForward, &_localForward);
	D3DXVec3Cross(&_localRight, &_localUp, &_localForward);
	D3DXVec3Cross(&_localUp, &_localForward, &_localRight);
	D3DXVec3Normalize(&_localRight, &_localRight);
	D3DXVec3Normalize(&_localUp, &_localUp);

	// -- Create Both World Matrices -- //
	_objectOrientedWorld = _screenScaleMatrix * Math::CreateWorld(&_position, &_localForward, &_localUp);
	_axisAlignedWorld = _screenScaleMatrix * Math::CreateWorld(&_position, &D3DXVECTOR3(SceneWorld._31, SceneWorld._32, SceneWorld._33), &D3DXVECTOR3(SceneWorld._21, SceneWorld._22, SceneWorld._23));

#if false
	if (ActiveSpace == TransformSpace::World ||
		ActiveMode == Mode::Rotate ||
		ActiveMode == Mode::NonUniformScale ||
		ActiveMode == Mode::UniformScale)
	{
		_gizmoWorld = _axisAlignedWorld;

		// align lines, boxes etc. with the grid-lines
		//memcpy_s(&_rotationMatrix, (16 * 3), SceneWorld, (16 * 3));
		_rotationMatrix._11 = SceneWorld._11;
		_rotationMatrix._12 = SceneWorld._12;
		_rotationMatrix._13 = SceneWorld._13;
		_rotationMatrix._21 = SceneWorld._21;
		_rotationMatrix._22 = SceneWorld._22;
		_rotationMatrix._23 = SceneWorld._23;
		_rotationMatrix._31 = SceneWorld._31;
		_rotationMatrix._32 = SceneWorld._32;
		_rotationMatrix._33 = SceneWorld._33;
	}
	else
	{
		_gizmoWorld = _objectOrientedWorld;

		// align lines, boxes etc. with the selected object
		_rotationMatrix._11 = _localRight.x;
		_rotationMatrix._12 = _localRight.y;
		_rotationMatrix._13 = _localRight.z;
		_rotationMatrix._21 = _localUp.x;
		_rotationMatrix._22 = _localUp.y;
		_rotationMatrix._23 = _localUp.z;
		_rotationMatrix._31 = _localForward.x;
		_rotationMatrix._32 = _localForward.y;
		_rotationMatrix._33 = _localForward.z;
	}
#else
	_gizmoWorld = _objectOrientedWorld;

	// align lines, boxes etc. with the selected object
	_rotationMatrix._11 = _localRight.x;
	_rotationMatrix._12 = _localRight.y;
	_rotationMatrix._13 = _localRight.z;
	_rotationMatrix._21 = _localUp.x;
	_rotationMatrix._22 = _localUp.y;
	_rotationMatrix._23 = _localUp.z;
	_rotationMatrix._31 = _localForward.x;
	_rotationMatrix._32 = _localForward.y;
	_rotationMatrix._33 = _localForward.z;
#endif
	// -- Reset Colors to default -- //
	ApplyColor(Axis::X, _axisColors[0]);
	ApplyColor(Axis::Y, _axisColors[1]);
	ApplyColor(Axis::Z, _axisColors[2]);

	// -- Apply Highlight -- //
	ApplyColor(ActiveAxis, _highlightColor);

	// Linecolor update
	{
		D3D11_MAPPED_SUBRESOURCE subResource;
		HRESULT hr = D3D::GetDC()->Map
		(
			_translationLineVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
		);

		memcpy(subResource.pData, &_translationLineVertices, sizeof(VertexColor) * 18);

		D3D::GetDC()->Unmap(_translationLineVertexBuffer, 0);
	}
}

void GizmoComponent::Render()
{
	if (!_isActive) return;

	UINT stride = 0;
	UINT offset = 0;

	worldBuffer->SetMatrix(_gizmoWorld);
	worldBuffer->SetVSBuffer(1);

	depthMode[1]->OMSetDepthStencilState();
	#pragma region Draw: Axis-Lines
	{
		rasterizerState[1]->RSSetState();

		stride = sizeof(VertexColor);
		offset = 0;

		D3D::GetDC()->IASetVertexBuffers(0, 1, &_translationLineVertexBuffer, &stride, &offset);
		D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		lineShader->Render();

		D3D::GetDC()->Draw(18, 0);
		rasterizerState[0]->RSSetState();
	}
	#pragma endregion

	switch (ActiveMode)
	{
		case Mode::NonUniformScale:
		case Mode::Translate:
			switch (ActiveAxis)
			{
				#pragma region Draw Quads
				case Axis::ZX:
				case Axis::YZ:
				case Axis::XY:
					{
						//TODO: 알파블렌드 있어야 함.
						rasterizerState[1]->RSSetState();
						Quad* activeQuad;
						switch (ActiveAxis)
						{
							case Axis::XY:
								activeQuad = &_quads[0];
								break;
							case Axis::ZX:
								activeQuad = &_quads[1];
								break;
							case Axis::YZ:
								activeQuad = &_quads[2];
								break;
						}

						lineShader->Render(); // 이거 먼저 해줘야함.
						activeQuad->Render();

						rasterizerState[0]->RSSetState();
					}
					break;
				#pragma endregion
			}
			break;

		case Mode::UniformScale:
			#pragma region Draw Quads
			if (ActiveAxis != Axis::None)
			{
				//TODO: 알파블렌드 있어야 함.
				rasterizerState[1]->RSSetState();

				lineShader->Render();
				for (UINT i = 0; i < 3; i++)
				{
					_quads[i].Render();
				}

				rasterizerState[0]->RSSetState();
			}
			#pragma endregion
			break;
	}

	// draw the 3d meshes
	for (UINT i = 0; i < 3; i++)
	{
		GizmoModel* activeModel;
		activeModel = ActiveModels[(UINT)ActiveMode];

		D3DXCOLOR color;
		switch (ActiveMode)
		{
			case Mode::UniformScale:
				color = _axisColors[0];
				break;
			default:
				color = _axisColors[i];
				break;
		}

		activeModel->SetWorld(_modelLocalSpace[i] * _gizmoWorld);
		activeModel->SetColor(color);
		activeModel->Render();
	}

	// 안할꺼임
	//if (SelectionBoxesIsVisible)
	//	DrawSelectionBox();
	depthMode[0]->OMSetDepthStencilState();

	string sAxis;
	switch (ActiveAxis)
	{
		case Axis::X:
			sAxis = "X";
			break;
		case Axis::Y:
			sAxis = "Y";
			break;
		case Axis::Z:
			sAxis = "Z";
			break;
		case Axis::XY:
			sAxis = "XY";
			break;
		case Axis::YZ:
			sAxis = "YZ";
			break;
		case Axis::ZX:
			sAxis = "ZX";
			break;
	}
	ImGui::Text("Axis: %s", sAxis.c_str());
}

void GizmoComponent::AddEntity(GameRender * entity)
{
	_selectionPool.push_back(entity);
}

void GizmoComponent::SelectEntities(bool addToSelection, bool removeFromSelection)
{
	if (ActiveAxis == Axis::None)
	{
		if (!addToSelection && !removeFromSelection)
			Selection.clear();
		PickObject(removeFromSelection);
	}
	ResetDeltas();
}

void GizmoComponent::PickObject(bool removeFromSelection)
{
	if (_selectionPool.size() < 1)
	{
		assert(false);
	}
	D3DXVECTOR3 camPosition;
	values->MainCamera->Position(&camPosition);

	Ray ray;

	float closest = D3D11_FLOAT32_MAX;
	GameRender* obj = NULL;
	for (GameRender* entity : _selectionPool)
	{
		float intersection;
		ray.SettingRay(camPosition, values->MainCamera->Direction(values->Viewport, values->Perspective), entity->World());

		bool isIntersection = entity->Select(&ray, &intersection);

		if (isIntersection && intersection < closest)
		{
			bool find = false;
			int fIdx = -1;
			for (UINT i = 0; i < Selection.size(); i++)
			{
				if (Selection[i] == entity)
				{
					find = true;
					fIdx = i;
					break;
				}
			}

			if (!find)
			{
				obj = entity;
				closest = intersection;
			}
			if (removeFromSelection)
			{
				if (fIdx > -1)
				{
					Selection.erase((Selection.begin() + fIdx));
				}
			}
		}
	}
	if (obj != NULL)
		Selection.push_back(obj);
}

void GizmoComponent::ResetDeltas()
{
	_tDelta = D3DXVECTOR3(0, 0, 0);
	_lastIntersectionPosition = D3DXVECTOR3(0, 0, 0);
	_intersectPosition = D3DXVECTOR3(0, 0, 0);
}

void GizmoComponent::SelectAxis()
{
	if (!Enabled)
		return;

	float closestintersection = FLT_MAX;

	D3DXVECTOR3 camPosition;
	values->MainCamera->Position(&camPosition);
	Ray ray;

	if (ActiveMode == Mode::Translate)
	{
		ray.SettingRay(camPosition, values->MainCamera->Direction(values->Viewport, values->Perspective), _gizmoWorld);
	}
	else
	{
		D3DXMATRIX matWorld;
		D3DXMatrixIdentity(&matWorld);

		ray.SettingRay(camPosition, values->MainCamera->Direction(values->Viewport, values->Perspective), _gizmoWorld);
	}

	#pragma region X, Y, Z Boxes
	float distance;
	bool intersection;

	intersection = xAxisBox->Intersect(&ray, distance);
	if (intersection && distance < closestintersection)
	{
		ActiveAxis = Axis::X;
		closestintersection = distance;
	}

	intersection = yAxisBox->Intersect(&ray, distance);
	if (intersection && distance < closestintersection)
	{
		ActiveAxis = Axis::Y;
		closestintersection = distance;
	}

	intersection = zAxisBox->Intersect(&ray, distance);
	if (intersection && distance < closestintersection)
	{
		ActiveAxis = Axis::Z;
		closestintersection = distance;
	}
	#pragma endregion

	if (ActiveMode == Mode::Rotate || ActiveMode == Mode::UniformScale || ActiveMode == Mode::NonUniformScale)
	{
		#pragma region BoundingSphere

		D3DXVECTOR3 _center;

		D3DXVec3TransformCoord(&_center, &_translationLineVertices[1].Position, &_gizmoWorld);
		xSphere->Center = _center;
		xSphere->Radius = radius * _screenScale;

		D3DXVec3TransformCoord(&_center, &_translationLineVertices[7].Position, &_gizmoWorld);
		ySphere->Center = _center;
		ySphere->Radius = radius * _screenScale;

		D3DXVec3TransformCoord(&_center, &_translationLineVertices[13].Position, &_gizmoWorld);
		zSphere->Center = _center;
		zSphere->Radius = radius * _screenScale;

		intersection = xSphere->Intersect(&ray, distance);
		if (intersection && distance < closestintersection)
		{
			ActiveAxis = Axis::X;
			closestintersection = distance;
		}

		intersection = ySphere->Intersect(&ray, distance);
		if (intersection && distance < closestintersection)
		{
			ActiveAxis = Axis::Y;
			closestintersection = distance;
		}

		intersection = zSphere->Intersect(&ray, distance);
		if (intersection && distance < closestintersection)
		{
			ActiveAxis = Axis::Z;
			closestintersection = distance;
		}
		#pragma endregion
	}
	if (ActiveMode == Mode::Translate || ActiveMode == Mode::NonUniformScale || ActiveMode == Mode::UniformScale)
	{
		// if no axis was hit (x,y,z) set value to lowest possible to select the 'farthest' intersection for the XY,XZ,YZ boxes. 
		// This is done so you may still select multi-axis if you're looking at the gizmo from behind!
		if (closestintersection >= FLT_MAX)
			closestintersection = FLT_MIN;

		#pragma region BoundingBoxes

		intersection = xyBox->Intersect(&ray, distance);
		if (intersection && distance > closestintersection)
		{
			ActiveAxis = Axis::XY;
			closestintersection = distance;
		}

		intersection = xzAxisBox->Intersect(&ray, distance);
		if (intersection && distance > closestintersection)
		{
			ActiveAxis = Axis::ZX;
			closestintersection = distance;
		}

		intersection = yzBox->Intersect(&ray, distance);
		if (intersection && distance > closestintersection)
		{
			ActiveAxis = Axis::YZ;
			closestintersection = distance;
		}
		#pragma endregion
	}

	if (closestintersection >= FLT_MAX || closestintersection <= FLT_MIN)
		ActiveAxis = Axis::None;
}

void GizmoComponent::SetGizmoPosition()
{
	switch (ActivePivot)
	{
		case PivotType::ObjectCenter:
			if (Selection.size() > 0)
				_position = Selection[0]->Position();
			break;
		case PivotType::SelectionCenter:
			_position = GetSelectionCenter();
			break;
		case PivotType::WorldOrigin:
			_position = D3DXVECTOR3(SceneWorld._41, SceneWorld._42, SceneWorld._43);
			break;
	}

	_position += _translationDelta;
}

D3DXVECTOR3 GizmoComponent::GetSelectionCenter()
{
	D3DXVECTOR3 center = D3DXVECTOR3(0, 0, 0);

	if (Selection.size() == 0)
		return center;

	for (GameRender* selected : Selection)
	{
		center += selected->Position();
	}

	return center / (float)Selection.size();
}

void GizmoComponent::ApplyColor(GizmoComponent::Axis axis, D3DXCOLOR color)
{
	switch (ActiveMode)
	{
		case Mode::NonUniformScale:
		case Mode::Translate:
			switch (axis)
			{
				case Axis::X:
					ApplyLineColor(0, 6, color);
					break;
				case Axis::Y:
					ApplyLineColor(6, 6, color);
					break;
				case Axis::Z:
					ApplyLineColor(12, 6, color);
					break;
				case Axis::XY:
					ApplyLineColor(0, 4, color);
					ApplyLineColor(6, 4, color);
					break;
				case Axis::YZ:
					ApplyLineColor(6, 2, color);
					ApplyLineColor(12, 2, color);
					ApplyLineColor(10, 2, color);
					ApplyLineColor(16, 2, color);
					break;
				case Axis::ZX:
					ApplyLineColor(0, 2, color);
					ApplyLineColor(4, 2, color);
					ApplyLineColor(12, 4, color);
					break;
			}
			break;
		case Mode::Rotate:
			switch (axis)
			{
				case Axis::X:
					ApplyLineColor(0, 6, color);
					break;
				case Axis::Y:
					ApplyLineColor(6, 6, color);
					break;
				case Axis::Z:
					ApplyLineColor(12, 6, color);
					break;
			}
			break;
		case Mode::UniformScale:
			ApplyLineColor(0, 18,
				ActiveAxis == Axis::None ? _axisColors[0] : _highlightColor);
			break;
	}
}

void GizmoComponent::ApplyLineColor(UINT startIndex, UINT count, D3DXCOLOR color)
{
	for (UINT i = startIndex; i < (startIndex + count); i++)
	{
		_translationLineVertices[i].Color = color;
	}
}
