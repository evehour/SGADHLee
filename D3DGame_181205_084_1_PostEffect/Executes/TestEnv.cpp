#include "stdafx.h"
#include "TestEnv.h"

#include "./Environment/CubeSky.h"
#include "./Model/ModelMesh.h"
#include "./Model/Model.h"

#include "./Objects/GameModel.h"

#include "../Viewer/Camera.h"
#include "../Viewer/Perspective.h"

#include "../PostEffects/Blur.h"

TestEnv::TestEnv()
{
	BuildGeometry();

	D3DDesc desc;
	D3D::GetDesc(&desc);
	dsv = new DepthStencilView(desc.Width, desc.Height, DXGI_FORMAT_D32_FLOAT);
	viewport = new Viewport(desc.Width, desc.Height);

	rtv = new RenderTargetView(desc.Width, desc.Height, DXGI_FORMAT_R8G8B8A8_UNORM);
	rtv2 = new RenderTargetView(desc.Width, desc.Height, DXGI_FORMAT_R8G8B8A8_UNORM);

	blur = new Blur(Shaders + L"058_Blur.fx", desc.Width, desc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, 1);
	blur->SetWeights(4.0f);

	render2D = new Render2D();
	render2D->Position(0, 0);
	render2D->Scale(desc.Width, desc.Height);

	render2D2 = new Render2D();
	render2D2->Position(0, 0);
	render2D2->Scale(desc.Width, desc.Height);

	aroundDegree = 0.0f;
}

TestEnv::~TestEnv()
{
	SAFE_DELETE(blur);

	SAFE_DELETE(render2D2);
	SAFE_DELETE(render2D);

	SAFE_DELETE(plane);
	SAFE_DELETE(planeMaterial);

	SAFE_DELETE(box);
	SAFE_DELETE(boxMaterial);

	SAFE_DELETE(quad);
	SAFE_DELETE(quadMaterial);

	for (int i = 0; i < 10; i++)
		SAFE_DELETE(cylinder[i]);
	SAFE_DELETE(cylinderMaterial);

	for (int i = 0; i < 10; i++)
		SAFE_DELETE(sphere[i]);

	SAFE_DELETE(sphereAround);

	SAFE_DELETE(sphereMaterial);

	SAFE_DELETE(bunny);

	SAFE_DELETE(sky);

	SAFE_DELETE(rtv);
	SAFE_DELETE(rtv2);

	SAFE_DELETE(dsv);
	SAFE_DELETE(viewport);
}

void TestEnv::Update()
{
	sky->Update();
	bunny->Update();

	float x = cosf(aroundDegree) * 5.0f;
	float z = sinf(aroundDegree) * 5.0f;
	aroundDegree += Time::Delta();
	sphereAround->Position(x, 3.0f, z);
}

void TestEnv::PreRender()
{
	viewport->RSSetViewport();
	
	D3D::Get()->SetRenderTarget(rtv->RTV(), dsv->DSV());
	D3D::Get()->Clear(D3DXCOLOR(1, 1, 1, 1), rtv->RTV(), dsv->DSV());
		
	sky->Render();

	plane->Render();
	box->Render();

	for (int i = 0; i < 10; i++)
		cylinder[i]->Render();

	for (int i = 0; i < 10; i++)
		sphere[i]->Render();

	sphereAround->Render();

	bunny->Render();

	D3D::Get()->SetRenderTarget();
}

void TestEnv::Render()
{
	//render2D2->SRV(rtv->SRV());
	//render2D2->Render();

	blur->Dispatch(rtv->SRV(), rtv->UAV());

	render2D->SRV(rtv->SRV());
	render2D->Render();
}

void TestEnv::PostRender()
{

}

void TestEnv::BuildGeometry()
{
	sky = new CubeSky(Textures + L"SunsetCube1024.dds");

	floor = Textures + L"Floor.png";
	stone = Textures + L"Stones.png";
	brick = Textures + L"Bricks.png";

	wstring shaderFile = Shaders + L"055_Mesh.fx";
	wstring shaderFile2 = Shaders + L"052_Mesh.fx";
	wstring shaderFile3 = Shaders + L"052_Mesh.fx";

	boxMaterial = new Material(shaderFile3);
	boxMaterial->SetDiffuseMap(stone);
	boxMaterial->SetAmbient(1.0f, 1.0f, 1.0f);
	boxMaterial->SetDiffuse(1.0f, 1.0f, 1.0f);
	boxMaterial->SetSpecular(0.8f, 0.8f, 0.8f, 16.0f);

	box = new MeshCube(boxMaterial, 1, 1, 1);
	box->Position(0, 0.5f, 0);
	box->Scale(5.0f, 1.0f, 5.0f);


	planeMaterial = new Material(shaderFile3);
	planeMaterial->SetDiffuseMap(floor);
	planeMaterial->SetAmbient(0.8f, 0.8f, 0.8f);
	planeMaterial->SetDiffuse(0.8f, 0.8f, 0.8f);
	planeMaterial->SetSpecular(0.0f, 0.0f, 0.0f, 1.0f);

	plane = new MeshGrid(planeMaterial, 60, 40, 20, 30, 3, 3);


	quadMaterial = new Material(shaderFile3);
	quadMaterial->SetDiffuseMap(floor);
	quadMaterial->SetAmbient(0.8f, 0.8f, 0.8f);
	quadMaterial->SetDiffuse(0.8f, 0.8f, 0.8f);
	quadMaterial->SetSpecular(0.0f, 0.0f, 0.0f, 1.0f);

	quad = new MeshQuad(quadMaterial);
	quad->Scale(5, 3, 1);
	quad->Position(0, 1.5f, -5.0f);


	cylinderMaterial = new Material(shaderFile);
	cylinderMaterial->SetDiffuseMap(brick);
	cylinderMaterial->SetAmbient(1.0f, 1.0f, 1.0f);
	cylinderMaterial->SetDiffuse(1.0f, 1.0f, 1.0f);
	cylinderMaterial->SetSpecular(0.8f, 0.8f, 0.5f, 16.0f);

	sphereMaterial = new Material(shaderFile2);
	sphereMaterial->SetDiffuseMap(stone);
	sphereMaterial->SetAmbient(0.6f, 0.8f, 0.9f);
	sphereMaterial->SetDiffuse(0.6f, 0.8f, 0.9f);
	sphereMaterial->SetSpecular(0.9f, 0.9f, 0.9f, 16.0f);

	wstring file = Textures + L"SunsetCube1024.dds";
	ID3D11ShaderResourceView* csrv;
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile
	(
		D3D::GetDevice(), (Textures + L"SunsetCube1024.dds").c_str(), NULL, NULL, &csrv, NULL
		//D3D::GetDevice(), file.c_str(), NULL, NULL, &srv, NULL
	);
	assert(SUCCEEDED(hr));

	for (UINT i = 0; i < 5; i++)
	{
		cylinder[i * 2] = new MeshCylinder(cylinderMaterial, 0.5f, 0.3f, 3.0f, 20, 20);
		cylinder[i * 2]->Position(-5.0f, 1.5f, -10.0f + i * 5.0f);

		cylinder[i * 2 + 1] = new MeshCylinder(cylinderMaterial, 0.5f, 0.3f, 3.0f, 20, 20);
		cylinder[i * 2 + 1]->Position(5.0f, 1.5f, -10.0f + i * 5.0f);


		sphere[i * 2] = new MeshSphere(sphereMaterial, 0.5f, 20, 20);
		sphere[i * 2]->Position(-5.0f, 3.5f, -10.0f + i * 5.0f);

		sphere[i * 2 + 1] = new MeshSphere(sphereMaterial, 0.5f, 20, 20);
		sphere[i * 2 + 1]->Position(5.0f, 3.5f, -10.0f + i * 5.0f);
	}

	sphereAround = new MeshSphere(sphereMaterial, 0.5f, 20, 20);
	sphereAround->Position(0, 3.0f, -5.0f);
	//sphereAround->Scale(1, 1, 1);
	

#define __USE_SPHERE_INSTEAD_BUNNY

	bunny = new GameModel
	(
		shaderFile,
#if !defined(__USE_SPHERE_INSTEAD_BUNNY)
		Materials + L"Meshes/", L"StanfordBunny.material",
		Models + L"Meshes/", L"StanfordBunny.mesh"
	);
	bunny->Scale(0.01f, 0.01f, 0.01f);
#else
		Materials + L"Meshes/", L"Sphere.material",
		Models + L"Meshes/", L"Sphere.mesh"
	);
	bunny->Scale(3, 3, 3);
#endif

	bunny->Position(0, 3.48f, 0);

	bunny->SetDiffuseMap(Textures + L"White.png");
	bunny->SetDiffuse(0.8f, 0.8f, 0.8f);
	bunny->SetSpecular(0.8f, 0.8f, 0.8f, 16.0f);
	
	for (ModelMesh* mesh : bunny->GetModel()->Meshes())
		mesh->Pass(1);

	SpotLightDesc spotLight;
	spotLight.Ambient = D3DXCOLOR(0, 0, 0, 1);
	spotLight.Diffuse = D3DXCOLOR(1, 0, 0, 1);
	spotLight.Specular = D3DXCOLOR(1, 0, 0, 1);
	spotLight.Attenuation = D3DXVECTOR3(1, 0, 0);
	spotLight.Spot = 20.0f;
	spotLight.Position = D3DXVECTOR3(0, 10, 0);
	spotLight.Direction = D3DXVECTOR3(0, -1, 0);

	Context::Get()->AddSpotLight(spotLight);
}
