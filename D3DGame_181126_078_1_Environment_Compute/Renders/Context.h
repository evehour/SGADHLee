#pragma once

class CBuffer;
class Context
{
public:
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

	GlobalLight* GetGlobalLight();
	void ChangeGlobalLight();

	class Viewport* GetViewport();
	void ChangeView();
	void ChangeView(Shader* shader, D3DXMATRIX valMartix);

	class Camera* GetMainCamera();

	void ViewProjectionSetting();

private:
	static Context* instance;

private:
	class Perspective* perspective;
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

public:
	struct GlobalLight
	{
		D3DXCOLOR Color = D3DXCOLOR(1, 1, 1, 1);
		D3DXVECTOR3 Direction = D3DXVECTOR3(-1, -1, -1);

		float Padding;
	};

private:
	GlobalLight light;
	map<Shader *, CBuffer*> lightMap;
};