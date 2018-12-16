#include "stdafx.h"
#include "NodeBase.h"

NodeBase::NodeBase()
	: parent(NULL)
{
}

NodeBase::~NodeBase()
{
	for (NodeBase* node : childList)
		SAFE_DELETE(node);
}

void NodeBase::AddChild(NodeBase * node)
{
	node->Parent(this);
	childList.push_back(node);
}

void NodeBase::InsertChild(int index, NodeBase * node)
{
	childList.insert((childList.begin() + index), node);
}

NodeBase * NodeBase::GetChild(int index)
{
	return *(childList.begin() + index);
}

void NodeBase::GetChildList(IN OUT vector<NodeBase*>& nodes)
{
	nodes.assign(childList.begin(), childList.end());
}

bool NodeBase::IsChild(NodeBase * node)
{
	bool bFind = false;
	for (NodeBase* inNode : childList)
	{
		if (inNode == node)
		{
			bFind = true;
			break;
		}
	}

	return bFind;
}

void NodeBase::RemoveChild(NodeBase * node, bool isRecursive)
{
	vector<NodeBase *>::iterator it;
	it = childList.begin();

	if (isRecursive)
	{
		node->RemoveAllChild(isRecursive);
	}

	while (it != childList.end())
	{
		if ((*it) == node)
		{
			childList.erase(it);
			node->Parent(NULL);
			break;
		}
		++it;
	}
}

void NodeBase::RemoveAllChild(bool isRecursive)
{
	if (isRecursive)
	{
		vector<NodeBase *>::iterator it;
		it = childList.begin();

		while (it != childList.end())
		{
			RemoveChild(*it, isRecursive);
		}

		childList.clear();
	}
}

void NodeBase::RemoveFromParent()
{
	if (parent != NULL)
	{
		parent->RemoveChild(this, false);
		parent = NULL;
	}
}
