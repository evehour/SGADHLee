#pragma once
#include "Execute.h"
#include "../Interfaces/ICommandPattern.h"

enum LANDSCAPE_EDIT_MODE
{
	L_EDIT_MODE_BRUSH = 0,
	L_EDIT_MODE_OBJECT,
	L_EDIT_MODE_MAX
};

enum LANDSCAPE_EDIT_MODE_SP
{
	L_EDIT_MODE_SP_NONE = 0,
	L_EDIT_MODE_SP_CREATE,
	L_EDIT_MODE_SP_MODIFY,
	L_EDIT_MODE_SP_REMOVE,
	L_EDIT_MODE_SP_MAX
};

class DrawLandscape : public Execute
{
public:
	DrawLandscape(ExecuteValues* values, class Terrain* terrain);
	~DrawLandscape();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	class Sky* sky;
	class Terrain* terrain;
	vector<class GameModel*> trees;
	vector<class DebugDraw*> debugDraws;
	D3DXCOLOR lineColor;

	LANDSCAPE_EDIT_MODE editMode;
	LANDSCAPE_EDIT_MODE_SP editModeSP;
	int createObjectCase;

	class GameModel* target;
	union TargetInfo
	{
		D3DXVECTOR3 Transform[3];
		struct
		{
			D3DXVECTOR3 Scale;
			D3DXVECTOR3 Rotation;
			D3DXVECTOR3 Translation;
		};

		TargetInfo() { memset(this, 0, sizeof(TargetInfo)); }
	} targetBeforeInfo;

	class RemoteController* remocon;
};

class Transformation
{
public:
	Transformation(class GameModel* target,
		D3DXVECTOR3 scale,
		D3DXVECTOR3 rotation,
		D3DXVECTOR3 translation);
	~Transformation() {}

	void Transformed();

	void InvTransformed();

	class GameModel* target;

private:
	D3DXVECTOR3 scale, scaleBef;
	D3DXVECTOR3 rotation, rotationBef;
	D3DXVECTOR3 translation, translationBef;

};

class TransformCommand : public ICommandPatternTransform
{
public:
	TransformCommand(class Transformation* transform) { this->transform = transform; };
	~TransformCommand() { SAFE_DELETE(this->transform); }

	void execute() override
	{
		transform->Transformed();
	}

	void undo() override
	{
		transform->InvTransformed();
	}
	void redo() override
	{
		transform->Transformed();
	}

	class GameModel* GetTarget() { return transform->target; }

private:
	class Transformation* transform;
};

class RemoteController
{
public:
	RemoteController() {};
	~RemoteController();


	// cases - 1: Undo,  2: Redo, F: All
	bool DoProcess(class GameModel* gameModel, int cases = 0);

	void AddUndoStack(TransformCommand* command);
	void ClearRedoStack();

private:
	vector<TransformCommand *> undoStack;
	vector<TransformCommand *> redoStack;

	bool Undo(class GameModel* gameModel);
	bool Redo(class GameModel* gameModel);
};