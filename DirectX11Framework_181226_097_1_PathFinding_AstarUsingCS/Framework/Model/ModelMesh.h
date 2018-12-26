#pragma once

class ModelMesh
{
public:
	friend class Model;
	friend class Models;

	void Render();

	wstring Name() { return name; }

	int ParentBoneIndex() { return parentBoneIndex; }
	class ModelBone* ParentBone() { return parentBone; }

public:
	void Clone(void** clone);

	void Pass(UINT val);

private:
	void Binding();

private:
	ModelMesh();
	~ModelMesh();

	wstring name;

	int parentBoneIndex;
	class ModelBone* parentBone;

	vector<class ModelMeshPart *> meshParts;

public:
	// HW
	vector<class ModelMeshPart *> GetMeshParts() { return meshParts; }
};