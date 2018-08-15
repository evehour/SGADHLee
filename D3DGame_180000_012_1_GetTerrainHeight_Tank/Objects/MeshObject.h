#pragma once
#include "GameModel.h"

class MeshObject : public GameModel
{
public:
	MeshObject(
		wstring matFolder, wstring matFile,
		wstring meshFolder, wstring meshFile);
	~MeshObject();

	void Update() override;
	void Render() override;

private:
};