#pragma once
class QuadTree
{
public:
	static const UINT DivideCount = 1024; // 1024

public:
	struct NodeType;

	QuadTree(class Terrain* terrain);
	~QuadTree();

	void Update();
	void Render();

private:
	void CalcMeshDimensions(UINT vertexCount, float& centerX, float& centerZ, float& meshWidth);
	void CreateTreeNode(struct NodeType* node, float positionX, float positionZ, float width);

	UINT ContainTriangleCount(float positionX, float positionZ, float width);
	bool IsTriangleContained(UINT index, float positionX, float positionZ, float width);

private:
	UINT triangleCount, drawCount;
	VertexTextureNormal* vertices;

	NodeType* parent;

private:
	struct NodeType
	{
		float x, z, width;
		UINT triangleCount;
		ID3D11Buffer *vertexBuffer, *indexBuffer;

		NodeType *childs[4]; // 자식노드 4개(4개로 나누니까 4개인거야. 옥희?)
	};
};