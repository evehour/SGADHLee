#include "Framework.h"
#include "Context.h"
#include "../Viewer/Perspective.h"
#include "../Viewer/Viewport.h"
#include "../Viewer/Freedom.h"

Context* Context::instance = NULL;

void Context::Create()
{
	assert(instance == NULL);

	instance = new Context();
}

void Context::Delete()
{
	SAFE_DELETE(instance);
}

Context * Context::Get()
{
	return instance;
}

void Context::Update()
{
	mainCamera->Update();
	mainCamera->Matrix(&perFrame.View);
	mainCamera->Position(&perFrame.ViewPosition);

	mainCamera->Forward(&perFrame.ViewDirection);
	perFrame.Time = Time::Get()->Running();

	for (pair<Shader *, CBuffer*> temp : perFrameMap)
		temp.second->Change();

	CBuffers::Update();
}

Context::Context()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	perspective = new Perspective(desc.Width, desc.Height);
	perspective->GetMatrix(&projection.Project);

	mainCamera = new Freedom();
	viewport = new Viewport(desc.Width, desc.Height);
}

Context::~Context()
{
	for (pair<Shader *, CBuffer*> temp : perFrameMap)
		SAFE_DELETE(temp.second);

	for (pair<Shader *, CBuffer*> temp : projectionMap)
		SAFE_DELETE(temp.second);

	for (pair<Shader *, CBuffer*> temp : lightMap)
		SAFE_DELETE(temp.second);

	for (pair<Shader *, CBuffer*> temp : pointLightMap)
		SAFE_DELETE(temp.second);

	for (pair<Shader *, CBuffer*> temp : spotLightMap)
		SAFE_DELETE(temp.second);


	SAFE_DELETE(mainCamera);
	SAFE_DELETE(perspective);
	SAFE_DELETE(viewport);
}

void Context::AddShader(Shader * shader)
{
	CBuffer* cbPerFrame = new CBuffer(shader, "CB_PerFrame", &perFrame, sizeof(PerFrame));
	perFrameMap.insert(pair<Shader *, CBuffer*>(shader, cbPerFrame));

	CBuffer* cbProjection = new CBuffer(shader, "CB_Projection", &projection, sizeof(Projection));
	projectionMap.insert(pair<Shader *, CBuffer*>(shader, cbProjection));

	CBuffer* cbLight = new CBuffer(shader, "CB_Light", &light, sizeof(GlobalLight));
	lightMap.insert(pair<Shader *, CBuffer*>(shader, cbLight));

	CBuffer* cbPointLight = new CBuffer(shader, "CB_PointLight", &pointLight, sizeof(PointLightBuffer));
	pointLightMap.insert(pair<Shader *, CBuffer*>(shader, cbPointLight));

	CBuffer* cbSpotLight = new CBuffer(shader, "CB_SpotLight", &spotLight, sizeof(SpotLightBuffer));
	spotLightMap.insert(pair<Shader *, CBuffer*>(shader, cbSpotLight));
}

Perspective * Context::GetPerspective()
{
	return perspective;
}

void Context::ChangePerspective()
{
	for (pair<Shader *, CBuffer*> temp : projectionMap)
		temp.second->Change();
}

void Context::ChangePerspective(Shader * shader, D3DXMATRIX valMartix)
{
	if (projectionMap.count(shader) < 1) return;
	D3DXMATRIX oldValue = projection.Project;
	projection.Project = valMartix;
	projectionMap.at(shader)->SelfChange();
	projection.Project = oldValue;
}

void Context::ChangePerspective(const D3DXMATRIX & valMartix)
{
	D3DXMATRIX oldValue = projection.Project;

	projection.Project = valMartix;
	for (pair<Shader *, CBuffer*> tmp : projectionMap)
		tmp.second->SelfChange();

	projection.Project = oldValue;
}

Viewport * Context::GetViewport()
{
	return viewport;
}

void Context::ChangeView()
{
	for (pair<Shader *, CBuffer*> temp : perFrameMap)
		temp.second->Change();
}

void Context::ChangeView(Shader * shader, D3DXMATRIX valMartix)
{
	if (perFrameMap.count(shader) < 1) return;
	D3DXMATRIX oldValue = perFrame.View;
	perFrame.View = valMartix;
	perFrameMap.at(shader)->SelfChange();
	perFrame.View = oldValue;
}

void Context::ChangeView(const D3DXMATRIX & valMartix)
{
	D3DXMATRIX oldValue = perFrame.View;

	perFrame.View = valMartix;
	for (pair<Shader *, CBuffer*> temp : perFrameMap)
		temp.second->SelfChange();

	perFrame.View = oldValue;
}

Camera * Context::GetMainCamera()
{
	return mainCamera;
}

void Context::ViewProjectionSetting()
{
}

Context::GlobalLight * Context::GetGlobalLight()
{
	return &light;
}

void Context::ChangeGlobalLight()
{
	float length = D3DXVec3Length(&light.Position);
	D3DXVECTOR3 target = light.Position + (light.Direction * length);

	D3DXMatrixLookAtLH(
		&light.LightViewMatrix,
		&light.Position,
		//&(light.Position + light.Direction),
		&D3DXVECTOR3(0, 0, 0),
		&D3DXVECTOR3(0, 1, 0)
	);

	D3DXVECTOR3 CenterLS;
	//D3DXVec3TransformCoord(&CenterLS, &target, &light.LightViewMatrix);
	D3DXVec3TransformCoord(&CenterLS, &D3DXVECTOR3(0, 0, 0), &light.LightViewMatrix);
	float l = CenterLS.x - length;
	float b = CenterLS.y - length;
	float n = CenterLS.z - length;
	float r = CenterLS.x + length;
	float t = CenterLS.y + length;
	float f = CenterLS.z + length;

	D3DXMATRIX p;
	D3DXMatrixOrthoOffCenterLH(&p, l, r, b, t, n, f);
	D3DXMATRIX T;
	D3DXMatrixIdentity(&T);
	T._11 = 0.5f;
	T._22 = -0.5f;
	T._33 = 1.0f;
	T._41 = 0.5f;
	T._42 = 0.5f;

	light.ShadowProject = p;
	light.ShadowMatrix = light.LightViewMatrix * light.ShadowProject * T;

	for (pair<Shader *, CBuffer*> temp : lightMap)
		temp.second->Change();
}

void Context::AddPointLight(PointLightDesc & light)
{
	pointLight.Lights[pointLight.Count] = light;
	pointLight.Count++;

	for (pair<Shader *, CBuffer*> temp : pointLightMap)
		temp.second->Change();
}

void Context::AddSpotLight(SpotLightDesc & light)
{
	spotLight.Lights[pointLight.Count] = light;
	spotLight.Count++;

	for (pair<Shader *, CBuffer*> temp : spotLightMap)
		temp.second->Change();
}