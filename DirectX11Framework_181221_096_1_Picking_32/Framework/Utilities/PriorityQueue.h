#pragma once

class IPriorityNode
{
public:
	virtual float Priority() = 0;
	virtual void Priority(float val) = 0;

	virtual int InsertionIndex() = 0;
	virtual void InsertionIndex(int val) = 0;

	virtual int QueueIndex() = 0;
	virtual void QueueIndex(int val) = 0;
//-----------------------------------------------------------------------------
//	virtual float Priority() override { return priority; }
//	virtual void Priority(float val) override { priority = val; }
//
//	virtual int InsertionIndex() override { return insertionIndex; }
//	virtual void InsertionIndex(int val) override { insertionIndex = val; }
//
//	virtual int QueueIndex() override { return queueIndex; }
//	virtual void QueueIndex(int val) override { queueIndex = val; }
//
//private:
//	float priority = 0;
//	float insertionIndex = 0;
//	float queueIndex = 0;
//-----------------------------------------------------------------------------
};

//template<typename T>
class PriorityQueue
{
public:
	PriorityQueue(int maxCount);
	~PriorityQueue();


	void Enqueue(IPriorityNode* node, float priority);
	IPriorityNode* Dequeue();

	bool Empty() { return count <= 1; }
	IPriorityNode* First() { return nodes[1]; }
	bool Contains(IPriorityNode* node) { return nodes[node->QueueIndex()] == node; }
	void Remove(IPriorityNode* node);

private:
	void Swap(IPriorityNode* node1, IPriorityNode* node2);
	void NodeUpdate(IPriorityNode* node);
	bool HigherPriority(IPriorityNode* higher, IPriorityNode* lower);
	void CascadeUp(IPriorityNode* node);
	void CascadeDown(IPriorityNode* node);

private:
	vector<IPriorityNode *> nodes;
	int nodeEverEnqueueCount;

	int count;
};