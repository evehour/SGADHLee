#include "stdafx.h"
#include "DrawLandscape.h"
#include "../Landscape/Sky.h"
#include "../Landscape/Terrain.h"
#include "../Objects/MeshSphere.h"
#include "../Objects/GameTree.h"
#include "../Executes/DebugDraw.h"


DrawLandscape::DrawLandscape(ExecuteValues * values, Terrain * terrain)
//DrawLandscape::DrawLandscape(ExecuteValues * values)
	: Execute(values)
	, editMode(L_EDIT_MODE_OBJECT), editModeSP(L_EDIT_MODE_SP_NONE)
	, createObjectCase(0)
{
	sky = new Sky(values);

#if true
	this->terrain = terrain;
#else
	//Create Terrain
	{
		wstring heightMap = Contents + L"HeightMaps/HeightMap256.png";
		Material* material = new Material(Shaders + L"022_TerrainBrushLine.hlsl");
		
		terrain = new Terrain(values, material);
	}
#endif

	//Create Trees
	{
		for (int i = 0; i < 10; i++)
		{
			GameModel* tree = new GameTree(values, Shaders + L"024_Billboard.hlsl", Textures + L"Tree.png");
			D3DXVECTOR3 P;
			P.x = Math::Random(0.0f, 255.0f);
			P.z = Math::Random(0.0f, 255.0f);
			P.y = terrain->Y(P);

			D3DXVECTOR3 S;
			S.x = Math::Random(10.0f, 30.0f);
			S.y = Math::Random(10.0f, 30.0f);
			S.z = 1;

			P.y += S.y * 0.5f;

			tree->Position(P);
			tree->Scale(S);
			
			trees.push_back(tree);
		}
	}

	//UN/RE-DO
	{
		remocon = new RemoteController();
	}

	//Create DebugDraw
	{
		for (UINT i = 0; i < 10; i++)
		{
			DebugDraw* d = new DebugDraw(values, Math::Random(0, 1));
			D3DXVECTOR3 P;
			P.x = Math::Random(0.0f, 255.0f);
			P.z = Math::Random(0.0f, 255.0f);
			P.y = terrain->Y(P);

			D3DXVECTOR3 S;
			float rS = Math::Random(1.0f, 10.0f);
			S.x = S.y = S.z = rS;

			P.y += S.y * 0.5f;

			d->Position(P);
			d->Scale(S);

			debugDraws.push_back(d);
		}
	}
}

DrawLandscape::~DrawLandscape()
{
	for (DebugDraw* d : debugDraws)
		SAFE_DELETE(d);

	SAFE_DELETE(remocon);

	for (UINT i = 0; i < trees.size(); i++)
	{
		if (trees[i] != NULL)
		{
			GameTree* t = (GameTree*)trees[i];
			SAFE_DELETE(t);
		}
	}

	SAFE_DELETE(sky);
}

void DrawLandscape::Update()
{	
	sky->Update();

	D3DXVECTOR3 position, S;
	switch (editMode)
	{
	case L_EDIT_MODE_BRUSH:
		terrain->Update();
		break;
	case L_EDIT_MODE_OBJECT:
		switch (editModeSP)
		{
		case L_EDIT_MODE_SP_NONE:
			target = NULL;
			break;
		case L_EDIT_MODE_SP_CREATE:
			target = NULL;
			if (Keyboard::Get()->Press(VK_LSHIFT) && Mouse::Get()->Down(0))
			{
				if (terrain->Y(&position))
				{
					GameTree* tree = new GameTree(values, Shaders + L"024_Billboard.hlsl", Textures + L"Tree.png");

					S.x = Math::Random(10.0f, 30.0f);
					S.y = Math::Random(10.0f, 30.0f);
					S.z = 1;

					position.y += S.y * 0.5f;

					tree->Position(position);
					tree->Scale(S);

					trees.push_back(tree);
				}
			}
			break;
		case L_EDIT_MODE_SP_MODIFY:
			if (Keyboard::Get()->Press(VK_LSHIFT) && Mouse::Get()->Down(0))
			{
				D3DXVECTOR3 start;
				values->MainCamera->Position(&start);

				D3DXVECTOR3 direction = values->MainCamera->Direction(
					values->Viewport, values->Perspective
				);

				for (GameModel* gm : trees)
				{
					if (gm->IsPick(start, direction, position))
					{
						if (target != NULL && target != gm)
						{
							target->Position(targetBeforeInfo.Translation);
							target->Scale(targetBeforeInfo.Scale);
							target->Rotation(targetBeforeInfo.Rotation);
						}
						target = gm;
						targetBeforeInfo.Translation= gm->Position();
						targetBeforeInfo.Scale = gm->Scale();
						targetBeforeInfo.Rotation= gm->Rotation();
						break;
					}
				}
			}
			break;
		case L_EDIT_MODE_SP_REMOVE:
			if (Keyboard::Get()->Press(VK_LSHIFT) && Mouse::Get()->Down(0))
			{
				D3DXVECTOR3 start;
				values->MainCamera->Position(&start);

				D3DXVECTOR3 direction = values->MainCamera->Direction(
					values->Viewport, values->Perspective
				);

				vector<GameModel*>::iterator iter = trees.begin();
				while (iter != trees.end())
				{
					if ((*iter)->IsPick(start, direction, position))
					{
						target = (*iter);
						break;
					}
					iter++;
				}
			}

			if (Keyboard::Get()->Down(VK_DELETE))
			{
				if (target != NULL)
				{
					vector<GameModel*>::iterator iter = trees.begin();
					while (iter != trees.end())
					{
						if(target == (*iter))
						{
							GameModel* gm = (*iter);
							iter = trees.erase(iter);
							SAFE_DELETE(gm);
							target = NULL;
							break;
						}
						iter++;
					}
				}
			}
			break;
		}//switch(editModeSP)
		break;
	}//switch(editMode)

	for (GameModel* tree : trees)
		tree->Update();

	if(remocon->DoProcess(target, 3))
	{
		targetBeforeInfo.Translation = target->Position();
		targetBeforeInfo.Scale = target->Scale();
		targetBeforeInfo.Rotation = target->Rotation();
	}

	for (DebugDraw* d : debugDraws)
		d->Update();
}

void DrawLandscape::PreRender()
{
	terrain->PreRender();
	ImGui::SliderInt("Edit Mode", (int*)&editMode, 0, L_EDIT_MODE_MAX - 1);
}

void DrawLandscape::Render()
{
	sky->Render(); // 젤먼저 그려야 함.

	terrain->Render();

	for (GameModel* tree : trees)
		tree->Render();

	for (DebugDraw* d : debugDraws)
		d->Render();
}

void DrawLandscape::PostRender()
{
	switch (editMode)
	{
	case L_EDIT_MODE_BRUSH:
		terrain->PostRender();
		break;
	case L_EDIT_MODE_OBJECT:
		char str[256];
		switch (editModeSP)
		{
		case L_EDIT_MODE_SP_NONE:
			sprintf_s(str, "Current Mode: NONE");
			break;
		case L_EDIT_MODE_SP_CREATE:
			sprintf_s(str, "Current Mode: CREATE");
			break;
		case L_EDIT_MODE_SP_MODIFY:
			sprintf_s(str, "Current Mode: MODIFY");
			break;
		case L_EDIT_MODE_SP_REMOVE:
			sprintf_s(str, "Current Mode: REMOVE");
			break;
		}

		int beforeVal = editModeSP;
		ImGui::Text(str);
		ImGui::SliderInt("", (int*)&editModeSP, L_EDIT_MODE_SP_NONE, L_EDIT_MODE_SP_MAX - 1);
		if (beforeVal != editModeSP) target = NULL;

		switch (editModeSP)
		{
		case L_EDIT_MODE_SP_CREATE:
			if (ImGui::RadioButton("Tree", &createObjectCase, 0))
			{
			}
			//else if (ImGui::RadioButton("Car", &createObjectCase, 1))
			//{
			//	int b = 10;
			//}
			break;
		case L_EDIT_MODE_SP_MODIFY:
			if (target != NULL)
			{
				D3DXVECTOR3 position = target->Position();
				D3DXVECTOR3 scale = target->Scale();
				D3DXVECTOR3 rotation = target->Rotation();

				ImGui::Text("Position");
				ImGui::SliderFloat3("T", (float*)&position, 0.0f, 256.0f);
				ImGui::Text("Scale");
				ImGui::SliderFloat3("S", (float*)&scale, 0.01f, 10.0f);
				//ImGui::Text("Rotation");
				//ImGui::SliderFloat3("R", (float*)&rotation, 0.0f, Math::ToRadian(360.0f));

				target->Position(position);
				target->Scale(scale);

				if (ImGui::Button("Apply"))
				{
					remocon->ClearRedoStack();

					TransformCommand* tc = new TransformCommand(
						new Transformation(
							target,
							targetBeforeInfo.Scale,
							targetBeforeInfo.Rotation,
							targetBeforeInfo.Translation)
					);
					tc->execute();
					remocon->AddUndoStack(tc);

					targetBeforeInfo.Translation = target->Position();
					targetBeforeInfo.Scale = target->Scale();
					targetBeforeInfo.Rotation = target->Rotation();
				}
			}
			break;
		case L_EDIT_MODE_SP_REMOVE:
			break;
		}

		ImGui::Separator();
		ImGui::Spacing();
		break;
	}
}

void DrawLandscape::ResizeScreen()
{
}



inline Transformation::Transformation(GameModel * target, D3DXVECTOR3 scale, D3DXVECTOR3 rotation, D3DXVECTOR3 translation)
	: target(target)
	, scaleBef(scale), rotationBef(rotation), translationBef(translation)
{
	this->scale = target->Scale();
	this->rotation = target->Rotation();
	this->translation = target->Position();
}

void Transformation::Transformed()
{
	if (target == NULL) return;

	target->Scale(scale);
	target->Rotation(rotation);
	target->Position(translation);
}

void Transformation::InvTransformed()
{
	if (target == NULL) return;

	target->Scale(scaleBef);
	target->Rotation(rotationBef);
	target->Position(translationBef);
}

RemoteController::~RemoteController()
{
	vector<TransformCommand *>::iterator iter;

	iter = undoStack.begin();
	while (iter != undoStack.end())
	{
		TransformCommand* tc = (*iter);
		SAFE_DELETE(tc);
		iter = undoStack.erase(iter);
	}

	iter = redoStack.begin();
	while (iter != redoStack.end())
	{
		TransformCommand* tc = (*iter);
		SAFE_DELETE(tc);
		iter = redoStack.erase(iter);
	}
}

bool RemoteController::Undo(GameModel * gameModel)
{
	if (Keyboard::Get()->Press(VK_LCONTROL) && Keyboard::Get()->Down('Z'))
	{
		if (undoStack.size() > 0)
		{
			vector<TransformCommand *>::iterator iter;
			iter = (undoStack.begin() + (undoStack.size() - 1));
			(*iter)->undo();
			gameModel = (*iter)->GetTarget();
			redoStack.push_back((*iter));
			undoStack.erase(iter);
			return true;
		}
	}
	return false;
}

bool RemoteController::Redo(GameModel * gameModel)
{
	if (Keyboard::Get()->Press(VK_LCONTROL) && Keyboard::Get()->Down('Y'))
	{
		if (redoStack.size() > 0)
		{
			vector<TransformCommand *>::iterator iter;
			iter = (redoStack.begin() + (redoStack.size() - 1));
			(*iter)->redo();
			gameModel = (*iter)->GetTarget();
			undoStack.push_back((*iter));
			redoStack.erase(iter);
			return true;
		}
	}
	return false;
}

// cases - 1: Undo,  2: Redo, F: All
bool RemoteController::DoProcess(GameModel * gameModel, int cases)
{
	bool b = false;

	if (cases & 1) b |= Undo(gameModel);
	if (cases & 2) b |= Redo(gameModel);

	return b;
}

void RemoteController::AddUndoStack(TransformCommand * command)
{
	undoStack.push_back(command);
}

void RemoteController::ClearRedoStack()
{
	if (redoStack.size() < 1) return;

	vector<TransformCommand *>::iterator iter = redoStack.begin();
	while (iter != redoStack.end())
	{
		TransformCommand* tc = (*iter);
		SAFE_DELETE(tc);
		iter = redoStack.erase(iter);
	}
}
