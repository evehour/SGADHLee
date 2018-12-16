#pragma once
#include "NodeBase.h"

class GameNode : public NodeBase
{
public:
	GameNode();
	virtual ~GameNode();

	string Name() { return name; }

	virtual void OnUpdate();
	virtual void OnReset(bool isRecursive);
	virtual void Update() {}
	virtual void Reset() {}

public:
	bool Enable() { return enable; }
	void Enable(bool val);

protected:
	bool enable;
	vector<function<void(bool)>> Enabled;
};