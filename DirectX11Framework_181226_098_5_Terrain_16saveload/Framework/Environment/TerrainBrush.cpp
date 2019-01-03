#include "Framework.h"
#include "Terrain.h"
#include "TerrainBrush.h"

#include "Utilities/BinaryFile.h"

TerrainBrush::TerrainBrush(class Terrain* terrain, float heightMapPixelWidth, float heightMapPixelHeight)
	: brushScale(1, 1), terrain(terrain)
	, heightMapWidth(heightMapPixelWidth), heightMapHeight(heightMapPixelHeight)
	, brushImage(NULL), render2DBrush(NULL), render2DProjBrushOnTerrain(NULL), rtvBrush(NULL)
	, pickCopyMap(NULL), pickCopySRV(NULL)
{
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
}

void TerrainBrush::Initialize()
{
}

void TerrainBrush::Ready()
{
	terrainShader = terrain->GetShader();
	SettingBrush();
	SettingBrushMap();


	Terrain::InitDesc desc;
	desc = terrain->Desc();
	brushShader->AsScalar("MaxHeight")->SetFloat(desc.HeightScale);
	brushShader->AsVector("HeightMapPixSize")->SetFloatVector(D3DXVECTOR2(heightMapWidth, heightMapHeight));

	groupW = (UINT)ceil(heightMapWidth / 16);
	groupH = (UINT)ceil(heightMapHeight / 16);
}

void TerrainBrush::SaveHeightMap(wstring saveFile)
{
	Terrain::InitDesc desc;
	desc = terrain->Desc();

	UINT dataSize = (UINT)(heightMapWidth * heightMapHeight);
	float* retData = new float[dataSize];

	CsResource* output = new CsResource(sizeof(float), dataSize, NULL);

	D3D::GetDC()->CopyResource(pickCopyMap, terrain->GetHeightMapTexture());
	brushShader->AsShaderResource("OriginMap")->SetResource(pickCopySRV);
	brushShader->AsUAV("SaveData")->SetUnorderedAccessView(output->UAV());

	brushShader->Dispatch(0, 3, groupW, groupH, 1);

	output->Read(retData);
	brushShader->AsShaderResource("OriginMap")->SetResource(NULL);
	brushShader->AsUAV("SaveData")->SetUnorderedAccessView(NULL);
	SAFE_DELETE(output);

	BYTE* writeDatas = new BYTE[dataSize];

	for (UINT i = 0; i < dataSize; i++)
	{
		float val = retData[i];
		BYTE val2 = (BYTE)retData[i];
		writeDatas[i] = (BYTE)retData[i];
	}

	BinaryWriter* w = new BinaryWriter();
	w->Open(saveFile);

	w->Byte(writeDatas, dataSize);

	w->Close();
	SAFE_DELETE(w);
	SAFE_DELETE_ARRAY(writeDatas);
	SAFE_DELETE_ARRAY(retData);
}

void TerrainBrush::SetBrush(Texture * const t)
{
	brushImage = t;
	render2DBrush->SRV(brushImage->SRV());
	terrainShader->AsShaderResource("BrushTexture")->SetResource(brushImage->SRV());
}

void TerrainBrush::UpdatePosition(const D3DXVECTOR3 & pixelPosition)
{
	pickPixelPosition = D3DXVECTOR2(pixelPosition.x, pixelPosition.z);
	terrainShader->AsVector("PickPosition")->SetFloatVector(pickPixelPosition);
	render2DBrush->Position(pickPixelPosition);
}

void TerrainBrush::Scale(const D3DXVECTOR2 & scale)
{
	brushScale = scale;
	render2DBrush->Scale(brushScale);
	brushShader->AsVector("BrushRate")->SetFloatVector(brushScale);
	terrainShader->AsVector("BrushRate")->SetFloatVector(scale);
}

void TerrainBrush::Power(float val)
{
	brushPower = val < 0.05f ? 0.05f : val;
	brushShader->AsScalar("BrushPower")->SetFloat(brushPower * (bBrushUp ? 1.0f : -1.0f));
}

void TerrainBrush::Brush(BrushType brushType, const bool & bUp)
{
	rtvBrush->Clear();
	D3D::Get()->SetRenderTarget(rtvBrush->RTV(), rtvBrush->DSV());
	rtvBrush->GetViewport()->RSSetViewport();
	render2DProjBrushOnTerrain->Render();
	render2DBrush->Render();
	D3D::Get()->SetRenderTarget();

	D3D::GetDC()->CopyResource(pickCopyMap, terrain->GetHeightMapTexture());

	brushShader->AsShaderResource("OriginMap")->SetResource(pickCopySRV);
	brushShader->AsShaderResource("ProjBrushMap")->SetResource(rtvBrush->SRV());
	brushShader->AsUAV("Output")->SetUnorderedAccessView(terrain->GetHeightMapUAV());

	brushShader->AsVector("PickPixelPos")->SetFloatVector(pickPixelPosition);
	if (bBrushUp != bUp)
	{
		bBrushUp = bUp;
		brushShader->AsScalar("BrushPower")->SetFloat(brushPower * (bBrushUp ? 1.0f : -1.0f));
	}

	brushShader->Dispatch(0, (UINT)brushType, groupW, groupH, 1);

	brushShader->AsShaderResource("OriginMap")->SetResource(nullptr);
	brushShader->AsShaderResource("ProjBrushMap")->SetResource(nullptr);
	brushShader->AsUAV("Output")->SetUnorderedAccessView(nullptr);
	brushShader->Apply(0, (UINT)brushType, true);

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
	render2DProjBrushOnTerrain = new Render2D(2, heightMapWidth, heightMapHeight);
	render2DBrush = new Render2D(1, heightMapWidth, heightMapHeight);
	rtvBrush = new RenderTargetView((UINT)heightMapWidth, (UINT)heightMapHeight);

	render2DProjBrushOnTerrain->SRV(terrain->GetHeightMapSRV());
	render2DProjBrushOnTerrain->Scale(heightMapWidth, heightMapHeight);

	render2DBrush->UseCenterPosition(true);
	render2DBrush->Scale(brushScale);

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
	tex2dDesc.Format = DXGI_FORMAT_R32_FLOAT;
	tex2dDesc.SampleDesc.Count = 1;
	tex2dDesc.SampleDesc.Quality = 0;
	tex2dDesc.Width = (UINT)heightMapWidth;
	tex2dDesc.Height = (UINT)heightMapHeight;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.MiscFlags = 0;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

	float* datas = new float[tex2dDesc.Width * tex2dDesc.Height];

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = datas;
	data.SysMemPitch = tex2dDesc.Width * sizeof(float);

	HRESULT hr = D3D::GetDevice()->CreateTexture2D(&tex2dDesc, &data, &pickCopyMap);
	assert(SUCCEEDED(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = tex2dDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	hr = D3D::GetDevice()->CreateShaderResourceView(pickCopyMap, &srvDesc, &pickCopySRV);
	assert(SUCCEEDED(hr));

	SAFE_DELETE_ARRAY(datas);
}
