#pragma once
#include "Interfaces/ICloneable.h"

class ModelMeshPart : public ICloneable
{
public:
	friend class Model;
	friend class Models;
	friend class ModelMesh;

private:
	ModelMeshPart();
	~ModelMeshPart();

	void Binding();
	void Render();
	//파트 단위가 아니라 메쉬단위로 렌더링 될거라 private으로 선언된 것임.
	//때문에 ModelMesh에 월드가 있는 것.

	void Clone(void** clone);
public:
	vector<ModelVertexType> GetVertices() { return vertices; }

private:
	Material* material;
	wstring materialName;

	class ModelMesh* parent;

	vector<ModelVertexType> vertices;
	vector<UINT> indices;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
};