#pragma once
#include "Execute.h"

class DrawLandscape : public Execute
{
public:
	DrawLandscape(ExecuteValues* values);
	~DrawLandscape();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	class Shader* specular;
	class Shader* normalMap;
	class Shader* pointLighting;
	class Shader* spotLighting;
	class Shader* areaLighting;

	D3DXCOLOR diffuseColor;
	D3DXCOLOR specularColor;
	float shininess;

	class Sky* sky;
	class Terrain* terrain;
	class MeshSphere* sphere;
	class MeshCube* cube;
	vector<class Billboard*> trees;

	class PointLight* pointLight;
	class SpotLight* spotLight;
	class AreaLight* areaLight;


private:
	class NormalBuffer : public ShaderBuffer
	{
	public:
		NormalBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.VisibleNormal = 1;
		}

		struct Struct
		{
			int VisibleNormal;

			float Padding[3];
		} Data;
	};
	NormalBuffer* normalBuffer;
};