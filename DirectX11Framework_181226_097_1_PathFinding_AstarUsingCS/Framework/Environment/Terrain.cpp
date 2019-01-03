#include "Framework.h"
#include "Terrain.h"

#include "HeightMap.h"
#include "TerrainRender.h"
#include "Systems/Window.h"

#include "Renders/CSResource.h"

const UINT Terrain::CellsPerPatch = 64;

Terrain::Terrain(InitDesc & desc)
	: desc(desc)
{
}

Terrain::~Terrain()
{
	SAFE_DELETE(render);
	SAFE_DELETE(heightMap);

	SAFE_DELETE(shaderCS);
	SAFE_DELETE(mousePickData);
}

void Terrain::Initialize()
{
	render = new TerrainRender(this);

	heightMap = new HeightMap((UINT)(this->desc.HeightMapWidth), (UINT)(this->desc.HeightMapHeight), this->desc.HeightScale);
	heightMap->Load(this->desc.HeightMap);

	Loading->AdjPercent(50);

	render->Initialize();

	Loading->AdjPercent(90);

	D3DDesc desc;
	D3D::GetDesc(&desc);

	dispatchX = ((UINT)desc.Width / 512) + 1;
	dispatchY = (UINT)desc.Height;
}

void Terrain::Ready(Material* material)
{
	desc.material = material;
	render->Ready(desc.material);

	shaderCS = new Shader(Shaders + L"803_CS_Terrain.fx", false);
	mousePickData = new CsResource(sizeof(D3DXCOLOR), 1, NULL);
}

void Terrain::Update()
{
	render->Update();
}

void Terrain::Render(UINT pass)
{
	render->Render(pass);
}

void Terrain::ResizeScreen()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	dispatchX = ((UINT)desc.Width / 512) + 1;
	dispatchY = (UINT)desc.Height;
}

void Terrain::Data(UINT row, UINT col, float data)
{
	heightMap->Data(row, col, data);
}

float Terrain::Data(UINT row, UINT col)
{
	return heightMap->Data(row, col);
}

float Terrain::Width()
{
	return (desc.HeightMapWidth - 1) * desc.CellSpacing;
}

float Terrain::Depth()
{
	return (desc.HeightMapHeight - 1) * desc.CellSpacing;
}

float Terrain::GetHeight(float x, float z)
{
	float c = (x + 0.5f * Width()) / desc.CellSpacing;
	float d = (z - 0.5f * Depth()) / -desc.CellSpacing;
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	float h00 = heightMap->Data(row, col);
	float h01 = heightMap->Data(row, col + 1);
	float h10 = heightMap->Data((row + 1), col);
	float h11 = heightMap->Data((row + 1), col + 1);

	float s = c - col;
	float t = d - row;

	float uy = 0.0f, vy = 0.0f;

	if (s + t <= 1.0f)
	{
		uy = h01 - h00;
		vy = h01 - h11;
	}
	else {
		uy = h10 - h11;
		vy = h01 - h11;
	}

	return h11 + (1.0f - s) * uy + (1.0f - t) * vy;
}

bool Terrain::Picking(OUT D3DXVECTOR3 * pickPixel, OUT D3DXVECTOR3 * pickWorldPos)
{
	float x, z;
	D3DXVECTOR3 position = Mouse::Get()->GetPosition();
	D3DXVECTOR4 result = { 0, 0, 0, 0 };
#if 0
	result = Texture::ReadPixel128(render->GetRenderTargetTexture(), (UINT)position.x, (UINT)position.y);
#else
	shaderCS->AsVector("mPosition")->SetFloatVector(position);
	shaderCS->AsShaderResource("InputTex")->SetResource(render->GetRenderTargetSRV());
	shaderCS->AsUAV("Output")->SetUnorderedAccessView(mousePickData->UAV());

	shaderCS->Dispatch(0, 0, dispatchX, dispatchY, 1);

	mousePickData->Read(&result);
#endif
	x = result.x;
	z = result.y;

	if (pickPixel != NULL)
	{
		pickPixel->x = x * (desc.HeightMapWidth - 1.0f);
		pickPixel->z = z * (desc.HeightMapHeight - 1.0f);
	}

	if (pickWorldPos != NULL)
	{
		float w, d;
		w = Width();
		d = Depth();
		pickWorldPos->x = x * w - (w * 0.5f);
		pickWorldPos->z = z * d - (d * 0.5f);
	}
	shaderCS->AsShaderResource("InputTex")->SetResource(NULL);
	shaderCS->AsUAV("Output")->SetUnorderedAccessView(NULL);

	return x + z > 0;
}

ID3D11Texture2D * Terrain::GetHeightMapTexture()
{
	if (render)
		return render->GetHeightMapTexture();
	else
		return nullptr;
}

ID3D11ShaderResourceView * Terrain::GetHeightMapSRV()
{
	if (render)
		return render->GetHeightMapSRV();
	else
		return nullptr;
}

ID3D11UnorderedAccessView * Terrain::GetHeightMapUAV()
{
	if (render)
		return render->GetHeightMapUAV();
	else
		return nullptr;
}
