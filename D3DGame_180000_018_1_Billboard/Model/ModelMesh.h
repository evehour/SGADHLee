#pragma once
#include "../Interfaces/ICloneable.h"

class ModelMesh : public ICloneable
{
public:
	friend class Model;
	friend class Models;

public:
	void SetWorld(D3DXMATRIX& world)
	{
		worldBuffer->SetMatrix(world);
	}

	void Render();

	wstring Name() { return name; }

	int ParentBoneIndex() { return parentBoneIndex; }
	class ModelBone* ParentBone() { return parentBone; }
	vector<class ModelMeshPart *> GetMeshParts() { return meshParts; }

public:
	void Clone(void** clone);

private:
	void Binding();

private:
	ModelMesh();
	~ModelMesh();

	wstring name;

	int parentBoneIndex;
	class ModelBone* parentBone;

	WorldBuffer* worldBuffer;
	vector<class ModelMeshPart *> meshParts;
};