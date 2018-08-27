#pragma once

class RenderSettings
{
public:
	static RenderSettings* Get();

	static void Create();
	static void Delete();

	static class Camera* MainCamera();
	static class Viewport* MainViewport();
	static class Perspective* MainProjection();
	static void RenderMainViewProjection();

	static LightBuffer* Light();
	static void RenderLight();

	static ViewProjectionBuffer* ViewProjection();
	static void RenderViewProjection();

	class Camera* GetCamera(UINT index) { return cameras[index]; }
	void SetCamera(UINT index);
	UINT AddCamera(class Camera* camera);

	class Viewport* GetViewport(UINT index) { return viewports[index]; }
	void SetViewport(UINT index, UINT slot = 0);
	UINT AddViewport(class Viewport* viewport);

	class Projection* GetProjection(UINT index) { return projections[index]; }
	void SetProjection(UINT index);
	UINT AddProjection(class Projection* matrix);

private:
	RenderSettings();
	~RenderSettings();

	static RenderSettings* instance;

	class LightBuffer* globalLight;
	class ViewProjectionBuffer * viewProjection;

	UINT currentCamera;
	vector<class Camera *> cameras;

	UINT currentViewport;
	vector<class Viewport *> viewports;

	UINT currentProjection;
	vector<class Projection *> projections;
};