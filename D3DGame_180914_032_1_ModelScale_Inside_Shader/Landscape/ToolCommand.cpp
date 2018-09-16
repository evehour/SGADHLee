#include "stdafx.h"
#include "ToolCommand.h"
#include "../Objects/GameModel.h"

CreateObject::CreateObject(ICOMMAND_PATTERN_TYPE & type, vector<GameModel*>* objects, GameModel * object)
	:object(object), objects(objects), isRemove(false)
{
	this->type = type;
}

CreateObject::~CreateObject()
{
	if (isRemove)
		SAFE_DELETE(object);
}

void CreateObject::execute()
{
	objects->push_back(object);
	isRemove = false;
}

void CreateObject::undo()
{
	vector<GameModel *>::iterator iter;
	for (iter = objects->begin(); iter != objects->end(); ++iter)
	{
		if (*iter == object)
		{
			objects->erase(iter);
			break;
		}
	}
	isRemove = true;
}

///////////////////////////////////////////////////////////////////////////////

RemoveObject::RemoveObject(const ICOMMAND_PATTERN_TYPE & type, vector<GameModel*>* objects, GameModel * object)
	: object(object), objects(objects), isCreate(false)
{
	this->type = type;
}

RemoveObject::~RemoveObject()
{
	if (!isCreate)
		SAFE_DELETE(object);
}

void RemoveObject::execute()
{
	vector<GameModel* >::iterator iter;
	for (iter = objects->begin(); iter != objects->end(); ++iter)
	{
		if (*iter == object)
		{
			objects->erase(iter);
			break;
		}
	}
	isCreate = false;
}

void RemoveObject::undo()
{
	objects->push_back(object);
	isCreate = true;
}

///////////////////////////////////////////////////////////////////////////////

ModifyObject::ModifyObject(const ICOMMAND_PATTERN_TYPE & type, const int & typeSub, GameModel * const object, const ModifyObjectInfo& mod)
	: typeSub(typeSub), object(object)
{
	this->type = type;

	//beforeInfo.Scale = object->Scale();
	beforeInfo.Rotation = object->Rotation();
	beforeInfo.Position = object->Position();
	
	memcpy(&afterInfo, &mod, sizeof(ModifyObjectInfo));
}

ModifyObject::~ModifyObject()
{
}

void ModifyObject::execute()
{
	if (typeSub & MODIFY_TYPE_SCALE) object->Scale(afterInfo.Scale);
	if (typeSub & MODIFY_TYPE_ROTATION) object->Rotation(afterInfo.Rotation);
	if (typeSub & MODIFY_TYPE_POSITION) object->Position(afterInfo.Position);
}

void ModifyObject::undo()
{
	if (typeSub & MODIFY_TYPE_SCALE)	object->Scale(beforeInfo.Scale);
	if (typeSub & MODIFY_TYPE_ROTATION) object->Rotation(beforeInfo.Rotation);
	if (typeSub & MODIFY_TYPE_POSITION) object->Position(beforeInfo.Position);
}
