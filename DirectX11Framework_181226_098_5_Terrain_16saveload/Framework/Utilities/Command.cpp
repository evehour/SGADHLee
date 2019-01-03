#include "Framework.h"
#include "Command.h"

CommandTerrain::CommandTerrain(ID3D11Texture2D * befTex2D, ID3D11Texture2D* afterTex2D, Shader* cs, ID3D11UnorderedAccessView* terrainUAV)
	: beforeTexture(befTex2D), afterTexture(afterTex2D)
	, brushCShader(cs), terrainUAV(terrainUAV)
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC srcDesc;
	befTex2D->GetDesc(&srcDesc);

	ID3D11Texture2D* copyTexture;
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = srcDesc.Width;
	textureDesc.Height = srcDesc.Height;
	textureDesc.MipLevels = srcDesc.MipLevels;
	textureDesc.ArraySize = srcDesc.ArraySize;
	textureDesc.Format = srcDesc.Format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.MiscFlags = srcDesc.MiscFlags;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	hr = D3D::GetDevice()->CreateTexture2D(&textureDesc, NULL, &copyTexture);
	assert(SUCCEEDED(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = srcDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	D3D::GetDC()->CopyResource(copyTexture, befTex2D);
	hr = D3D::GetDevice()->CreateShaderResourceView(copyTexture, &srvDesc, &beforeSRV);
	assert(SUCCEEDED(hr));
	SAFE_RELEASE(copyTexture);

	hr = D3D::GetDevice()->CreateTexture2D(&textureDesc, NULL, &copyTexture);
	assert(SUCCEEDED(hr));
	D3D::GetDC()->CopyResource(copyTexture, afterTex2D);
	hr = D3D::GetDevice()->CreateShaderResourceView(copyTexture, &srvDesc, &afterSRV);
	assert(SUCCEEDED(hr));
	SAFE_RELEASE(copyTexture);
}

CommandTerrain::~CommandTerrain()
{
	SAFE_RELEASE(beforeSRV);
	SAFE_RELEASE(afterSRV);
}

int CommandTerrain::Execute()
{
	brushCShader->AsShaderResource("OriginMap")->SetResource(afterSRV);
	brushCShader->AsUAV("Output")->SetUnorderedAccessView(terrainUAV);
	brushCShader->Dispatch(0, 4, (UINT)ceil(2049 / 16), (UINT)ceil(2049 / 16), 1);
	brushCShader->AsShaderResource("OriginMap")->SetResource(NULL);
	brushCShader->AsUAV("Output")->SetUnorderedAccessView(NULL);
	brushCShader->Apply(0, 4, true);

	return 0;
}

int CommandTerrain::Undo()
{
	brushCShader->AsShaderResource("OriginMap")->SetResource(beforeSRV);
	brushCShader->AsUAV("Output")->SetUnorderedAccessView(terrainUAV);
	brushCShader->Dispatch(0, 4, (UINT)ceil(2049 / 16), (UINT)ceil(2049 / 16), 1);
	brushCShader->AsShaderResource("OriginMap")->SetResource(NULL);
	brushCShader->AsUAV("Output")->SetUnorderedAccessView(NULL);
	brushCShader->Apply(0, 4, true);

	return 0;
}
