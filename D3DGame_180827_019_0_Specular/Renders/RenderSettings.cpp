#include "stdafx.h"
#include "RenderSettings.h"
#include "../Viewer/Freedom.h"
#include "../Viewer/Perspective.h"
#include "../Viewer/Viewport.h"

RenderSettings* RenderSettings::instance = NULL;

RenderSettings * RenderSettings::Get()
{
	assert(instance != NULL);

	return instance;
}

void RenderSettings::Create()
{
	assert(instance == NULL);

	instance = new RenderSettings();
}

void RenderSettings::Delete()
{
	SAFE_DELETE(instance);
}

Camera * RenderSettings::MainCamera()
{
	return instance->cameras[0];
}

Viewport * RenderSettings::MainViewport()
{
	return instance->viewports[0];
}

Perspective * RenderSettings::MainProjection()
{
	return (Perspective *)instance->projections[0];
}

void RenderSettings::RenderMainViewProjection()
{
	instance->currentCamera = 0;
	instance->currentProjection = 0;

	instance->RenderViewProjection();
}

RenderSettings::RenderSettings()
	: currentCamera(0)
	, currentViewport(0)
	, currentProjection(0)
{
	globalLight = new LightBuffer();
	viewProjection = new ViewProjectionBuffer();

	Freedom* camera = new Freedom();
	AddCamera(camera);

	D3DDesc desc;
	D3D::GetDesc(&desc);

	Viewport* viewport = new Viewport(desc.Width, desc.Height);
	AddViewport(viewport);
	viewport->RSSetViewport();

	Perspective* perspective = new Perspective(desc.Width, desc.Height, (float)D3DX_PI * 0.25f, 0.1f, 1000.0f);
	AddProjection(perspective);
}

RenderSettings::~RenderSettings()
{
	for (Camera* camera : cameras)
		SAFE_DELETE(camera);

	SAFE_DELETE(globalLight);
}

LightBuffer * RenderSettings::Light()
{
	return instance->globalLight;
}

void RenderSettings::RenderLight()
{
	instance->globalLight->SetPSBuffer(0);
}

void RenderSettings::SetProjection(UINT index)
{
	currentProjection = index;

	RenderViewProjection();
}

ViewProjectionBuffer * RenderSettings::ViewProjection()
{
	return instance->viewProjection;
}

void RenderSettings::RenderViewProjection()
{
	D3DXMATRIX view;
	instance->cameras[instance->currentCamera]->Matrix(&view);
	instance->viewProjection->SetView(view);

	D3DXMATRIX projection;
	instance->projections[instance->currentProjection]->Matrix(&projection);
	instance->viewProjection->SetProjection(projection);

	instance->viewProjection->SetVSBuffer(0);
}

UINT RenderSettings::AddProjection(Projection * projection)
{
	projections.push_back(projection);

	return projections.size() - 1;
}

UINT RenderSettings::AddCamera(Camera * camera)
{
	cameras.push_back(camera);

	return cameras.size() - 1;
}

void RenderSettings::SetViewport(UINT index, UINT slot)
{
	currentViewport = index;

	RenderViewProjection();
}

UINT RenderSettings::AddViewport(Viewport * viewport)
{
	viewports.push_back(viewport);

	return viewports.size() - 1;
}

void RenderSettings::SetCamera(UINT index)
{
	currentCamera = index;

	RenderViewProjection();
}
