#include "stdafx.h"
#include "GameNode.h"

GameNode::GameNode()
	: enable(true)
{
}

GameNode::~GameNode()
{
}

void GameNode::OnUpdate()
{
	Update();

	if (enable)
	{
		for (NodeBase* node : childList)
		{
			GameNode* gameNode = dynamic_cast<GameNode*>(node);
			if (gameNode != nullptr)
				gameNode->OnUpdate();
		}
	}
}

void GameNode::OnReset(bool isRecursive)
{
	Reset();

	if (isRecursive)
	{
		for (NodeBase* node : childList)
		{
			GameNode* gameNode = dynamic_cast<GameNode*>(node);
			if (gameNode == nullptr)
				assert(false);

			gameNode->Reset();
		}
	}
}

void GameNode::Enable(bool val)
{
	enable = val;

	for(auto temp : Enabled)
		temp(val);
}
