#pragma once

class CBuffer;

struct PointLightDesc
{
	D3DXCOLOR Ambient;
	D3DXCOLOR Diffuse;
	D3DXCOLOR Specular;

	D3DXVECTOR3 Position;
	float Range;

	D3DXVECTOR3 Attenuation;
	float Padding;
};

struct SpotLightDesc
{
	D3DXCOLOR Ambient;
	D3DXCOLOR Diffuse;
	D3DXCOLOR Specular;

	D3DXVECTOR3 Position;
	float Padding;

	D3DXVECTOR3 Direction;
	float Spot;

	D3DXVECTOR3 Attenuation;
	float Padding2;
};

class Context
{
private:
	struct GlobalLight;

public:
	static void Create();
	static void Delete();

	static Context* Get();

private:
	Context();
	~Context();

public:
	void Update();
	void AddShader(Shader* shader);

	class Perspective* GetPerspective();
	void ChangePerspective();
	void ChangePerspective(Shader* shader, D3DXMATRIX valMartix);
	void ChangePerspective(const D3DXMATRIX & valMartix);

	GlobalLight* GetGlobalLight();
	void ChangeGlobalLight();

	void AddPointLight(PointLightDesc& light);
	void AddSpotLight(SpotLightDesc& light);

	class Viewport* GetViewport();
	void ChangeView();
	void ChangeView(Shader* shader, D3DXMATRIX valMartix);
	void ChangeView(const D3DXMATRIX & valMartix);

	class Camera* GetMainCamera();

	void ViewProjectionSetting();

private:
	static Context* instance;

private:
	class Perspective* perspective;
	class Perspective* perspectiveS;
	class Viewport* viewport;
	class Camera* mainCamera;

private:
	struct PerFrame
	{
		D3DXMATRIX View;

		D3DXVECTOR3 ViewDirection;
		float Time;
		D3DXVECTOR3 ViewPosition;
		float Padding;
	};
	PerFrame perFrame;
	map<Shader*, CBuffer*> perFrameMap;

	struct Projection
	{
		D3DXMATRIX Project;
	};
	Projection projection;
	map<Shader *, CBuffer*> projectionMap;

private:
	struct GlobalLight
	{
		D3DXCOLOR Ambient;
		D3DXCOLOR Diffuse;
		D3DXCOLOR Specular;
		D3DXVECTOR3 Direction;
		float Padding;
		D3DXVECTOR3 Position;
		float Padding2;
		D3DXMATRIX LightViewMatrix;
		D3DXMATRIX ShadowProject;
		D3DXMATRIX ShadowMatrix;

		GlobalLight()
			: Ambient(0.2f, 0.2f, 0.2f, 1)
			, Diffuse(0.5f, 0.5f, 0.5f, 1)
			, Specular(0.5f, 0.5f, 0.5f, 1)
			, Direction(-1, -1, -1)
			, Position(10, 10, 10/*60, 80*/)
		{
			D3DXMatrixLookAtLH(
				&LightViewMatrix,
				&Position,
				//&(Position + Direction),
				&D3DXVECTOR3(0, 0, 0),
				&D3DXVECTOR3(0, 1, 0)
			);

			D3DXMatrixIdentity(&ShadowProject);
			D3DXMatrixIdentity(&ShadowMatrix);
		}
	};
	GlobalLight light;
	map<Shader *, CBuffer*> lightMap;

private:
	struct PointLightBuffer
	{
		PointLightDesc Lights[16];
		int Count = 0;

		float Padding[3];
	};
	PointLightBuffer pointLight;
	map<Shader *, CBuffer*> pointLightMap;

private:
	struct SpotLightBuffer
	{
		SpotLightDesc Lights[16];
		int Count = 0;

		float Padding[3];
	};
	SpotLightBuffer spotLight;
	map<Shader *, CBuffer*> spotLightMap;

public:
	struct Projection GetProjection() { return projection; }
};