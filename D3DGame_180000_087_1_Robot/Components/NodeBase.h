#pragma once

class NodeBase
{
public:
	NodeBase();
	virtual ~NodeBase();

public:
	virtual string Name() = 0;
	void Name(IN string var) { name = var; }

	NodeBase* Parent() { return parent; }
	void Parent(IN NodeBase * parent) { this->parent = parent; }

	UINT ChildCount() { return childList.size(); }
	void AddChild(NodeBase* node);
	void InsertChild(int index, NodeBase* node);
	NodeBase* GetChild(int index);
	void GetChildList(IN OUT vector<NodeBase*>& nodes);
	bool IsChild(NodeBase* node);
	void RemoveChild(NodeBase* node, bool isRecursive);
	void RemoveAllChild(bool isRecursive);
	void RemoveFromParent();

protected:
	string name;
	NodeBase* parent;

	vector<NodeBase *> childList;
};