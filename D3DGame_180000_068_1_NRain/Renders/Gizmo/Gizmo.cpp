#include "stdafx.h"
#include "Gizmo.h"
#include "GizmoAxis.h"

#include "./Boundings/Ray.h"
#include "./Boundings/BBox.h"
#include "./Boundings/BSphere.h"

Gizmo::Gizmo(ExecuteValues* values)
	: values(values)
	,lineOffset(1.0f), lineLength(3.0f), singleAxisThickness(0.2f)
{
	halfLineOffset = lineOffset / 2;

	xAxisBox = new BBox(
		D3DXVECTOR3(lineOffset, 0, 0),
		D3DXVECTOR3(lineOffset + lineLength, singleAxisThickness, singleAxisThickness));
	yAxisBox = new BBox(
		D3DXVECTOR3(0, lineOffset, 0),
		D3DXVECTOR3(singleAxisThickness, lineOffset + lineLength, singleAxisThickness));
	zAxisBox = new BBox(
		D3DXVECTOR3(0, 0, lineOffset),
		D3DXVECTOR3(singleAxisThickness, singleAxisThickness, lineOffset + lineLength));

	for (int i = 0; i < 3; i++)
		axis[i] = new DebugLine();

	axiss.push_back(new GizmoAxis(GizmoAxis::Kind::X, lineOffset, halfLineOffset, lineLength));
	axiss.push_back(new GizmoAxis(GizmoAxis::Kind::Y, lineOffset, halfLineOffset, lineLength));
	axiss.push_back(new GizmoAxis(GizmoAxis::Kind::Z, lineOffset, halfLineOffset, lineLength));

	axisLine = new DebugLine();
	UpdateLine();

	axisColor[(int)Axis::X] = D3DXCOLOR(1, 0, 0, 1);
	axisColor[(int)Axis::Y] = D3DXCOLOR(0, 1, 0, 1);
	axisColor[(int)Axis::Z] = D3DXCOLOR(0, 0, 1, 1);
	highlightColor = D3DXCOLOR(1, 1, 0, 1);

	visible = false;
	enable = false;

	depthMode[0] = new DepthStencilState();
	depthMode[1] = new DepthStencilState();
	depthMode[1]->DepthEnable(false);

	render = NULL;

	visible = true;








	D3DXMatrixIdentity(&_rotationMatrix);
	D3DXMatrixIdentity(&_rotationDelta);
	D3DXMatrixIdentity(&SceneWorld);
}

Gizmo::~Gizmo()
{
	SAFE_DELETE(xAxisBox);
	SAFE_DELETE(yAxisBox);
	SAFE_DELETE(zAxisBox);

	SAFE_DELETE(axisLine);

	for (ILine* line : axiss)
		SAFE_DELETE(line);

	SAFE_DELETE(depthMode[0]);
	SAFE_DELETE(depthMode[1]);

	for (int i = 0; i < 3; i++)
		SAFE_DELETE(axis[i]);
}

void Gizmo::Set(GameRender* render)
{
	this->render = render;

	Position(render->Position());

	UpdateLine();
}

void Gizmo::Update()
{
	if (visible)
	{
		_lastIntersectionPosition = _intersectPosition;
		if (Mouse::Get()->Press(0) && ActiveAxis != Axis::None)
		{
			D3DXVECTOR3 tPosition = Position();
			D3DXMATRIX invMat;
			D3DXMatrixInverse(&invMat, NULL, &_rotationMatrix);
			D3DXVec3TransformCoord(&tPosition, &tPosition, &invMat);


			D3DXVECTOR3 delta = D3DXVECTOR3(0, 0, 0);
			Ray ray;
			D3DXVECTOR3 camPos;
			values->MainCamera->Position(&camPos);
			ray.SettingRay(camPos, values->MainCamera->Direction(values->Viewport, values->Perspective), World());

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
								D3DXVECTOR3 vForward = Direction();
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
						_translationDelta = delta;
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
							break;
						case Axis::Y:
							D3DXMatrixRotationAxis(&deltaRot, &D3DXVECTOR3(_rotationMatrix._21, _rotationMatrix._22, _rotationMatrix._23), delta);
							rot *= deltaRot;
							break;
						case Axis::Z:
							D3DXMatrixRotationAxis(&deltaRot, &D3DXVECTOR3(_rotationMatrix._31, _rotationMatrix._32, _rotationMatrix._33), delta);
							rot *= deltaRot;
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
				Mouse::Get()->GetPosition();
				//SelectAxis(mousePosition);
			}
		}
	}

	UpdateLine();
}

void Gizmo::UpdateLine()
{
	axisLine->Draw(World(), axiss);

	axis[0]->Draw(World(), xAxisBox);
	axis[1]->Draw(World(), yAxisBox);
	axis[2]->Draw(World(), zAxisBox);
}

void Gizmo::Render()
{
	if (visible) {
		if (render != NULL) {
			ImGui::Begin("Gizmo");

			D3DXVECTOR3 pos, rot, scale;
			pos = render->Position();
			rot = render->Rotation();
			scale = render->Scale();

			if (ImGui::DragFloat3("Position", (float*)&pos, 0.1f)) {
				render->Position(pos);
				Position(pos);
				UpdateLine();
			}
			if (ImGui::DragFloat3("Rotation", (float*)&rot, 0.1f)) {
				render->RotationDegree(rot);
				//UpdateLine();
			}
			if (ImGui::DragFloat3("Scale", (float*)&scale, 0.1f)) {
				render->Scale(scale);
				//UpdateLine();
			}

			ImGui::End();
		}

		depthMode[1]->OMSetDepthStencilState();
		// axis
		{
			for (int i = 0; i < 3; i++)
				axis[i]->Render();

			// X
			axisLine->Color(axisColor[(int)Axis::X]);
			axisLine->Render(6, 0);
			// Y
			axisLine->Color(axisColor[(int)Axis::Y]);
			axisLine->Render(6, 6);
			// Z
			axisLine->Color(axisColor[(int)Axis::Z]);
			axisLine->Render(6, 12);
		}
		depthMode[0]->OMSetDepthStencilState();
	}
}

void Gizmo::SelectEntities(bool addToSelection, bool removeFromSelection)
{
	if (ActiveAxis == Axis::None)
	{
		if (!addToSelection && !removeFromSelection)
			Selection.clear();
		PickObject(removeFromSelection);
	}
	ResetDeltas();
}

void Gizmo::PickObject(bool removeFromSelection)
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
	for(GameRender* entity : _selectionPool)
	{
		float intersection;
		bool isIntersection = entity->Select(&ray, &intersection);

		ray.SettingRay(camPosition, values->MainCamera->Direction(values->Viewport, values->Perspective), entity->World());
		if (isIntersection && intersection < closest)
		{
			bool find = false;
			int fIdx = -1;
			for (int i = 0; i < Selection.size(); i++)
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

void Gizmo::ResetDeltas()
{
	_tDelta = D3DXVECTOR3(0, 0, 0);
	_lastIntersectionPosition = D3DXVECTOR3(0, 0, 0);
	_intersectPosition = D3DXVECTOR3(0, 0, 0);
}
