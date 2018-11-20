#include "stdafx.h"
#include "Context.h"
#include "../Viewer/Perspective.h"
#include "../Viewer/Viewport.h"
#include "../Viewer/Freedom.h"

Perspective* Context::perspective = NULL;
Viewport* Context::viewport = NULL;
Camera* Context::mainCamera = NULL;

void Context::Create()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	perspective = new Perspective(desc.Width, desc.Height);
	viewport = new Viewport(desc.Width, desc.Height);
	mainCamera = new Freedom();
}

void Context::Delete()
{
	SAFE_DELETE(mainCamera);
	SAFE_DELETE(perspective);
	SAFE_DELETE(viewport);
}

Perspective * Context::GetPerspective()
{
	return perspective;
}

Viewport * Context::GetViewport()
{
	return viewport;
}

Camera * Context::GetMainCamera()
{
	return mainCamera;
}

void Context::SetValues(Shader * shader)
{
	D3DXMATRIX V, P;
	mainCamera->Matrix(&V);
	perspective->GetMatrix(&P);

	shader->AsMatrix("View")->SetMatrix(V);
	shader->AsMatrix("Projection")->SetMatrix(P);
}
