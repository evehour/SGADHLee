#pragma once
#include "../Interfaces/ICommandPattern.h"

class GameModel;

enum MODIFY_TYPE
{
	MODIFY_TYPE_SCALE		= 0x0001,
	MODIFY_TYPE_ROTATION	= 0x0002,
	MODIFY_TYPE_POSITION	= 0x0004
};

class CreateObject : public ICommandPattern
{
public:
	CreateObject(ICOMMAND_PATTERN_TYPE& type, vector<GameModel* >* objects, GameModel* object);
	~CreateObject();

	void execute();

	void undo();
	void redo() { execute(); }

private:
	vector<GameModel* >* objects;
	GameModel* object;
	bool isRemove;
};

class RemoveObject : public ICommandPattern
{
public:
	RemoveObject(const ICOMMAND_PATTERN_TYPE& type, vector<GameModel* >* objects, GameModel* object);
	~RemoveObject();

	void execute();

	void undo();
	void redo() { execute(); }

private:
	vector<GameModel* >* objects;
	GameModel* object;
	bool isCreate;
};


class ModifyObject : public ICommandPattern
{
public:
	struct ModifyObjectInfo
	{
		D3DXVECTOR3 Scale;
		D3DXVECTOR3 Rotation;
		D3DXVECTOR3 Position;
	};

public:
	ModifyObject(const ICOMMAND_PATTERN_TYPE& type, const int& typeSub, GameModel* const object, const ModifyObjectInfo& mod);
	~ModifyObject();

	void execute();

	void undo();
	void redo() { execute(); }

private:
	UINT typeSub;
	GameModel* const object;
	ModifyObjectInfo beforeInfo;
	ModifyObjectInfo afterInfo;
};
