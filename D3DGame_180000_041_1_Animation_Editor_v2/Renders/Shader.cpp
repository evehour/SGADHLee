#include "stdafx.h"
#include "Shader.h"

vector<ShaderDesc> Shaders::descs;

Shader::Shader(wstring shaderFile, string vs, string ps)
{
	Shaders::Load(this, Shaders + shaderFile + L".hlsl", vs, ps);
}

Shader::~Shader()
{

}

void Shader::Render()
{
	D3D::GetDC()->IASetInputLayout(inputLayout);
	D3D::GetDC()->VSSetShader(vertexShader, NULL, 0);
	D3D::GetDC()->PSSetShader(pixelShader, NULL, 0);
}

void Shaders::Delete()
{
	for (ShaderDesc& desc : descs)
	{
		SAFE_RELEASE(desc.InputLayout);

		SAFE_RELEASE(desc.VertexShader);
		SAFE_RELEASE(desc.PixelShader);
	}
}

void Shaders::Load(Shader * shader, wstring file, string vs, string ps)
{
	ShaderDesc desc;
	desc.File = file;
	desc.Vs = vs;
	desc.Ps = ps;

	ShaderDesc exist;
	bool bExist = false;
	for (ShaderDesc temp : descs)
	{
		if (desc == temp)
		{
			bExist = true;
			exist = temp;

			break;
		}
	}

	if (bExist == false)
	{
		//Create VertexShader
		{
			ID3DBlob* blob = NULL;
			Compile(file, vs, "vs_5_0", &blob);

			HRESULT hr = D3D::GetDevice()->CreateVertexShader
			(
				blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &desc.VertexShader
			);
			assert(SUCCEEDED(hr));

			CreateInputLayout(blob, &desc.InputLayout);
			SAFE_RELEASE(blob);
		}

		//Create PixelShader
		{
			ID3DBlob* blob = NULL;
			Compile(file, ps, "ps_5_0", &blob);

			HRESULT hr = D3D::GetDevice()->CreatePixelShader
			(
				blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &desc.PixelShader
			);
			assert(SUCCEEDED(hr));

			SAFE_RELEASE(blob);
		}
		descs.push_back(desc);

		exist = desc;
	}

	shader->file = exist.File;
	shader->vs = exist.Vs;
	shader->ps = exist.Ps;

	shader->vertexShader = exist.VertexShader;
	shader->pixelShader = exist.PixelShader;
	shader->inputLayout = exist.InputLayout;
}

void Shaders::Compile(wstring file, string funcName, string profile, ID3DBlob** shaderBlob)
{
	ID3DBlob* error;
	HRESULT hr = D3DX11CompileFromFile
	(
		file.c_str(), NULL, NULL, funcName.c_str(), profile.c_str()
		, D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, shaderBlob, &error, NULL
	);

	if (FAILED(hr))
	{
		if (error != NULL)
		{
			string str = (const char *)error->GetBufferPointer();

			MessageBoxA(NULL, str.c_str(), (funcName + " Error").c_str(), MB_OK);
		}
		assert(false);
	}
}

void Shaders::CreateInputLayout(ID3DBlob * blob, ID3D11InputLayout ** layout)
{
	HRESULT hr;
	ID3D11ShaderReflection* reflection;

	hr = D3DReflect
	(
		blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflection
	);
	assert(SUCCEEDED(hr));


	D3D11_SHADER_DESC shaderDesc;
	reflection->GetDesc(&shaderDesc);

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (UINT i = 0; i< shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		reflection->GetInputParameterDesc(i, &paramDesc);

		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		string temp = paramDesc.SemanticName;
		if (temp == "POSITION")
			elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;

		inputLayoutDesc.push_back(elementDesc);
	}

	hr = D3D::GetDevice()->CreateInputLayout
	(
		&inputLayoutDesc[0]
		, inputLayoutDesc.size()
		, blob->GetBufferPointer()
		, blob->GetBufferSize()
		, layout
	);
	assert(SUCCEEDED(hr));

	SAFE_RELEASE(reflection);
}