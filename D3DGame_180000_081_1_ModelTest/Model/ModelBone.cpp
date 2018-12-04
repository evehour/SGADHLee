#include "stdafx.h"
#include "ModelBone.h"

ModelBone::ModelBone()
	:parent(NULL)
{
	D3DXMatrixIdentity(&matFinal);
}

ModelBone::~ModelBone()
{
}

void ModelBone::SetParent(ModelBone* parent)
{
	if (parent == NULL) return;
	DelParent();

	parent->AddChild(this);
	this->parent = parent;
	this->parentIndex = parent->index;
}

void ModelBone::DelParent()
{
	if (parent != NULL)
		parent->DelChild(this->index);
	parent = NULL;
	parentIndex = -1;
}

void ModelBone::AddChild(ModelBone* child)
{
	if (child == NULL) return;

	auto it = childs.begin();
	while (it != childs.end())
	{
		if ((*it)->index == child->index)
			return;

		it++;
	}

	childs.push_back(child);
}

void ModelBone::DelChild(int childIdx)
{
	auto it = childs.begin();
	while (it != childs.end())
	{
		if ((*it)->index == childIdx)
		{
			it = childs.erase(it);
			return;
		}

		it++;
	}
}

void ModelBone::Clone(void ** clone)
{
	ModelBone* bone = new ModelBone();
	bone->index = index;
	bone->name = name;
	bone->parentIndex = parentIndex;
	bone->local = local;
	bone->global = global;

	*clone = bone;
}
