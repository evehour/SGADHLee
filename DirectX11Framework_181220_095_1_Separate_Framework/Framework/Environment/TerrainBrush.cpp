#include "Framework.h"
#include "Terrain.h"
#include "TerrainBrush.h"

TerrainBrush::TerrainBrush(class Terrain* terrain, Shader* terrainShader, float heightMapPixelWidth, float heightMapPixelHeight)
	: brushScale(1, 1), terrain(terrain), terrainShader(terrainShader)
	, heightMapWidth(heightMapPixelWidth), heightMapHeight(heightMapPixelHeight)
	, brushImage(NULL), render2DBrush(NULL), render2DProjBrushOnTerrain(NULL), rtvBrush(NULL)
	, pickCopyMap(NULL), pickCopySRV(NULL)
{
	SettingBrush();
	SettingBrushMap();
}

TerrainBrush::~TerrainBrush()
{
	SAFE_DELETE(testViewRender2D);

	SAFE_RELEASE(pickCopySRV);
	SAFE_RELEASE(pickCopyMap);
	SAFE_DELETE(rtvBrush);
	SAFE_DELETE(render2DProjBrushOnTerrain);
	SAFE_DELETE(render2DBrush);

	SAFE_DELETE(brushShader);

	ClearBrushes();
}

void TerrainBrush::AddBrush(const wstring & name, const wstring & textureFile)
{
	Texture* t = new Texture(textureFile);
	brushNames.push_back(name);
	brushes.insert(pair<wstring, Texture*>(name, t));
}

void TerrainBrush::ChangeBrush(const UINT & idx)
{
	if (idx >= brushNames.size())
		assert(false);

	ChangeBrush(brushNames[idx]);
}

void TerrainBrush::ChangeBrush(const wstring & name)
{
	if (brushes.find(name) == brushes.end())
		assert(false);

	brushImage = brushes[name];
	render2DBrush->SRV(brushImage->SRV());
}

void TerrainBrush::UpdatePosition(const D3DXVECTOR3 & pixelPosition)
{
	terrainShader->AsVector("PickPosition")->SetFloatVector(D3DXVECTOR2(pixelPosition.x, pixelPosition.z));
	render2DBrush->Position(pixelPosition.x, pixelPosition.z);
}

void TerrainBrush::Brush(const bool & bUp, const float & brushPower)
{
	rtvBrush->Clear();
	D3D::Get()->SetRenderTarget(rtvBrush->RTV(), rtvBrush->DSV());
	rtvBrush->GetViewport()->RSSetViewport();
	render2DProjBrushOnTerrain->Render();
	render2DBrush->Render();
	D3D::Get()->SetRenderTarget();

	D3D::GetDC()->CopyResource(pickCopyMap, terrain->GetHeightMapTexture());

	float tmp = brushPower < 0.05f ? 0.05f : brushPower;
	float power = tmp * (bUp ? 1 : -1);

	brushShader->AsScalar("BrushPower")->SetFloat(power);
	brushShader->AsShaderResource("OriginMap")->SetResource(pickCopySRV);
	brushShader->AsShaderResource("ProjBrushMap")->SetResource(rtvBrush->SRV());
	brushShader->AsUAV("Output")->SetUnorderedAccessView(terrain->GetHeightMapUAV());
	UINT groupW, groupH;
	groupW = (UINT)ceil(heightMapWidth / 16);
	groupH = (UINT)ceil(heightMapHeight / 16);

	render2DProjBrushOnTerrain->SRV(NULL);
	brushShader->Dispatch(0, 0, groupW, groupH, 1);
	render2DProjBrushOnTerrain->SRV(terrain->GetHeightMapSRV());
}

void TerrainBrush::Hovering(const bool & val)
{
	bHovering = val;
	terrainShader->AsScalar("IsHovering")->SetBool(bHovering);
}

void TerrainBrush::TestRender()
{
	D3D::Get()->SetRenderTarget();
	Context::Get()->GetViewport()->RSSetViewport();
	testViewRender2D->Render();
	D3D::Get()->SetRenderTarget();
}

void TerrainBrush::SettingBrush()
{
	render2DProjBrushOnTerrain = new Render2D(1, heightMapWidth, heightMapHeight);
	render2DBrush = new Render2D(0, heightMapWidth, heightMapHeight);
	rtvBrush = new RenderTargetView(heightMapWidth, heightMapHeight);

	render2DProjBrushOnTerrain->SRV(terrain->GetHeightMapSRV());
	render2DProjBrushOnTerrain->Scale(heightMapWidth, heightMapHeight);

	render2DBrush->UseCenterPosition(true);
	render2DBrush->Scale(100, 100);

	//render2DBrush->SRV(brushImage->SRV());
	AddBrush(L"Moon", Textures + L"Moon.png");
	ChangeBrush(0);

	testViewRender2D = new Render2D();
	testViewRender2D->Scale(600, 600);
	testViewRender2D->SRV(rtvBrush->SRV());
}

void TerrainBrush::SettingBrushMap()
{
	brushShader = new Shader(Shaders + L"803_CS_Terrain_ImageBrush.fx", false);

	D3D11_TEXTURE2D_DESC tex2dDesc;
	ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));
	tex2dDesc.ArraySize = 1;
	tex2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	tex2dDesc.CPUAccessFlags = 0;
	tex2dDesc.Format = DXGI_FORMAT_R16_FLOAT;
	tex2dDesc.SampleDesc.Count = 1;
	tex2dDesc.SampleDesc.Quality = 0;
	tex2dDesc.Width = heightMapWidth;
	tex2dDesc.Height = heightMapHeight;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.MiscFlags = 0;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

	float* datas = new float[tex2dDesc.Width * tex2dDesc.Height];
	D3DXFLOAT16* temp = new D3DXFLOAT16[tex2dDesc.Width * tex2dDesc.Height];
	D3DXFloat32To16Array(temp, datas, tex2dDesc.Width * tex2dDesc.Height);

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = temp;
	data.SysMemPitch = tex2dDesc.Width * sizeof(D3DXFLOAT16);

	HRESULT hr = D3D::GetDevice()->CreateTexture2D(&tex2dDesc, &data, &pickCopyMap);
	assert(SUCCEEDED(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R16_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	hr = D3D::GetDevice()->CreateShaderResourceView(pickCopyMap, &srvDesc, &pickCopySRV);
	assert(SUCCEEDED(hr));

	SAFE_DELETE_ARRAY(temp);
	SAFE_DELETE_ARRAY(datas);
}

void TerrainBrush::ClearBrushes()
{
	for (vector<wstring>::iterator it = brushNames.begin(); it != brushNames.end();)
	{
		SAFE_DELETE(brushes[*it]);
		brushes.erase(*it);
		it = brushNames.erase(it);
	}
}
