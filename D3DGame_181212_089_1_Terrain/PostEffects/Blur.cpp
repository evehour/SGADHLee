#include "stdafx.h"
#include "Blur.h"

Blur::Blur(wstring shaderFile, UINT width, UINT height, DXGI_FORMAT format, int blurCount)
	: width(width), height(height), blurCount(blurCount)
{
	shader = new Shader(shaderFile, false);
	shader->AsScalar("BlurRadius")->SetInt(blurCount);

	HRESULT hr;
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0; // 불가능
	desc.MiscFlags = 0;

	ID3D11Texture2D* texture;
	hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &texture);
	assert(SUCCEEDED(hr));

//-----------------------------------------------------------------------------

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	hr = D3D::GetDevice()->CreateShaderResourceView(texture, &srvDesc, &srv);
	assert(SUCCEEDED(hr));

//-----------------------------------------------------------------------------

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	uavDesc.Format = format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	
	hr = D3D::GetDevice()->CreateUnorderedAccessView(texture, &uavDesc, &uav);
	assert(SUCCEEDED(hr));

	SAFE_RELEASE(texture);
}

Blur::~Blur()
{
	SAFE_RELEASE(srv);
	SAFE_RELEASE(uav);
	SAFE_DELETE(shader);
}

void Blur::ChangeBlurCount(int blurCount, float sigma)
{
	this->blurCount = blurCount;
	shader->AsScalar("BlurRadius")->SetInt(blurCount);

	SetWeights(sigma);
}

void Blur::SetWeights(float sigma)
{
	float d = 2.0f * sigma * sigma;

	UINT arraySize = blurCount * 2 + 1;

	float* weights = new float[arraySize];
	float sum = 0.0f;

	for (int i = -blurCount; i <= blurCount; ++i)
	{
		weights[i + blurCount] = expf((float)i * (float)i / d);
		sum += weights[i + blurCount];
	}

	for (int i = 0; i < arraySize; i++)
	{
		weights[i] /= sum;
	}

	shader->AsScalar("Weights")->SetFloatArray(weights, 0, arraySize);
	SAFE_DELETE_ARRAY(weights);
}

void Blur::Dispatch(ID3D11ShaderResourceView * inputSRV, ID3D11UnorderedAccessView * inputUAV)
{
	UINT group = 0;

	for (int i = 0; i < blurCount; i++)
	{
		shader->AsShaderResource("Input")->SetResource(inputSRV);
		shader->AsUAV("Output")->SetUnorderedAccessView(uav, D3D11_KEEP_UNORDERED_ACCESS_VIEWS);

		// ceil 올림
		// floor 내림
		// round 반올림
		group = (UINT)ceil(width / 256.0f); // N값으로 나누어줌.
		shader->Dispatch(0, 0, group, height, 1); // height 갯수만큼 y가 실행되라고 하는 거임.


		shader->AsShaderResource("Input")->SetResource(srv);
		shader->AsUAV("Output")->SetUnorderedAccessView(inputUAV, D3D11_KEEP_UNORDERED_ACCESS_VIEWS);

		group = (UINT)ceilf(height / 256.0f); // N값으로 나누어줌.
		shader->Dispatch(0, 1, width, group, 1); // height 갯수만큼 y가 실행되라고 하는 거임.
	}


	D3D::GetDC()->CSSetShader(NULL, NULL, 0);
}
