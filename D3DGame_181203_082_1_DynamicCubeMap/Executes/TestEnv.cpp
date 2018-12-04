#include "stdafx.h"
#include "TestEnv.h"

#include "./Environment/CubeSky.h"
#include "./Model/ModelMesh.h"
#include "./Model/Model.h"

#include "./Objects/GameModel.h"

#include "../Viewer/Camera.h"
#include "../Viewer/Perspective.h"
TestEnv::TestEnv()
{
	BuildCubeTexture(300, 300);
	BuildGeometry();
	BuildViewMatrix(0, 3.48f, 0); // 토깽이 위치를 기준으로 6면을 따기 위해.

	render2D = new Render2D();
	render2D->Position(0, 0);
	render2D->Scale(300, 300);
	//render2D->SRV(srv);
	aroundDegree = 0.0f;
}

TestEnv::~TestEnv()
{
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

	SAFE_RELEASE(cubeTexture);
	for (UINT i = 0; i < 6; i++)
		SAFE_RELEASE(rtv[i]);
	SAFE_RELEASE(srv);

	SAFE_DELETE(dsv);
	SAFE_DELETE(viewport);
	SAFE_DELETE(perspective);
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

	D3DXMATRIX oView, oProj, proj;
	Context::Get()->GetMainCamera()->Matrix(&oView);
	Context::Get()->GetPerspective()->GetMatrix(&oProj);

	perspective->GetMatrix(&proj);
	
	Context::Get()->ChangePerspective(proj);

	for (int i = 0; i < 6; i++)
	{
		Context::Get()->ChangeView(view[i]);

		D3D::Get()->Clear(D3DXCOLOR(1, 1, 1, 1), rtv[i], dsv->DSV());
		D3D::Get()->SetRenderTarget(rtv[i], dsv->DSV());
		
		sky->Render();

		plane->Render();
		box->Render();

		for (int i = 0; i < 10; i++)
			cylinder[i]->Render();

		for (int i = 0; i < 10; i++)
			sphere[i]->Render();

		sphereAround->Render();

		bunny->Render();
	}

	Context::Get()->ChangePerspective();
	Context::Get()->ChangeView();
	Context::Get()->Update();
	
	D3D::GetDC()->GenerateMips(srv);
}

void TestEnv::Render()
{
	sky->Render();

	plane->Render();
	box->Render();

	for (int i = 0; i < 10; i++)
		cylinder[i]->Render();

	for (int i = 0; i < 10; i++)
		sphere[i]->Render();

	sphereAround->Render();
	
	bunny->Render();

	//quadMaterial->GetShader()->AsShaderResource("DiffuseMap")->SetResource(rtv->SRV());
	//quad->Render();

	//render2D->Render();
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

	wstring shaderFile = Shaders + L"057_DynamicCube.fx";
	wstring shaderFile2 = Shaders + L"055_MeshRef.fx";
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
	sphereMaterial->GetShader()->AsShaderResource("CubeMap")->SetResource(csrv);
	SAFE_RELEASE(csrv);

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

	for (Material* material : bunny->GetModel()->Materials())
		material->GetShader()->AsShaderResource("CubeMap")->SetResource(srv);

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

void TestEnv::BuildCubeTexture(UINT width, UINT height)
{
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//Create Texture2D
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 0;
		desc.ArraySize = 6;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Format = format;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

		HRESULT hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &cubeTexture);
		assert(SUCCEEDED(hr));
	}

	//Create RTV
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		desc.Format = format;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = 1; // 몇번째 붙을꺼냐에 해당하는 숫자
		desc.Texture2DArray.MipSlice = 0;
		for (UINT i = 0; i < 6; i++)
		{
			desc.Texture2DArray.FirstArraySlice = i;
			HRESULT hr = D3D::GetDevice()->CreateRenderTargetView(cubeTexture, &desc, &rtv[i]);
			assert(SUCCEEDED(hr));
		}
	}

	//Create SRV
	{
		// 1개만 있으면 됨.
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		desc.TextureCube.MostDetailedMip = 0; // 밉맵을 최대 몇개까지 지원할 것인지
		desc.TextureCube.MipLevels = -1;

		HRESULT hr = D3D::GetDevice()->CreateShaderResourceView(cubeTexture, &desc, &srv);
		assert(SUCCEEDED(hr));
	}

	
	dsv = new DepthStencilView(width, height, DXGI_FORMAT_D24_UNORM_S8_UINT);
	viewport = new Viewport(width, height);
}

void TestEnv::BuildViewMatrix(float x, float y, float z)
{
	D3DXVECTOR3 target[] =
	{
		D3DXVECTOR3(x + 1, y, z), // 좌
		D3DXVECTOR3(x - 1, y, z), // 우
		D3DXVECTOR3(x, y + 1, z), // 위
		D3DXVECTOR3(x, y - 1, z), // 아래
		D3DXVECTOR3(x, y, z + 1), // 후
		D3DXVECTOR3(x, y, z - 1), // 전
	};

	D3DXVECTOR3 up[] =
	{
		D3DXVECTOR3(0,1,0),
		D3DXVECTOR3(0,1,0),
		D3DXVECTOR3(0,0,-1),
		D3DXVECTOR3(0,0,1),
		D3DXVECTOR3(0,1,0),
		D3DXVECTOR3(0,1,0),
	};

	D3DXVECTOR3 position(x, y, z);

	for (UINT i = 0; i < 6; i++)
	{
		D3DXMatrixLookAtLH(&view[i], &position, &target[i], &up[i]);
	}

	//원근투영을 할 필요 없음. 그냥 비치는 곳을 하는것이기 때문에.
	perspective = new Perspective(1, 1, Math::PI * 0.5f, 0.1, 1000.0f);
}
