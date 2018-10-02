#include "stdafx.h"
#include "Material.h"

Material::Material()
	: shader(NULL)
	, diffuseMap(NULL), specularMap(NULL), normalMap(NULL), detailMap(NULL)
{
	buffer = new Buffer();
}

Material::Material(Shader* shader)
	: shader(shader)
	, diffuseMap(NULL), specularMap(NULL), normalMap(NULL), detailMap(NULL)
{
	buffer = new Buffer();
}

Material::~Material()
{
	SAFE_DELETE(normalMap)
	SAFE_DELETE(detailMap);
	SAFE_DELETE(diffuseMap);
	SAFE_DELETE(specularMap);
}

void Material::SetShader(Shader * shader)
{
	this->shader = shader;
}

void Material::SetDiffuseMap(string file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SetDiffuseMap(String::ToWString(file), info);
}

void Material::SetDiffuseMap(wstring file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SAFE_DELETE(diffuseMap);

	diffuseMap = new Texture(file, info);
}

void Material::SetSpecularMap(string file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SetSpecularMap(String::ToWString(file), info);
}

void Material::SetSpecularMap(wstring file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SAFE_DELETE(specularMap);

	specularMap = new Texture(file, info);
}

void Material::SetNormalMap(string file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SetNormalMap(String::ToWString(file), info);
}

void Material::SetNormalMap(wstring file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SAFE_DELETE(normalMap);

	normalMap = new Texture(file, info);
}

void Material::SetDetailMap(string file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SetDetailMap(String::ToWString(file), info);
}

void Material::SetDetailMap(wstring file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SAFE_DELETE(detailMap);

	detailMap = new Texture(file, info);
}

void Material::PSSetBuffer()
{
	if (shader != NULL)
		shader->Render();


	UINT slot = 0;
	if (diffuseMap != NULL)
	{
		diffuseMap->SetShaderResource(slot);
		diffuseMap->SetShaderSampler(slot);
	}
	else
	{
		Texture::SetBlankShaderResource(slot);
		Texture::SetBlankSamplerState(slot);
	}


	slot = 1;
	if (specularMap != NULL)
	{
		specularMap->SetShaderResource(slot);
		specularMap->SetShaderSampler(slot);
	}
	else
	{
		Texture::SetBlankShaderResource(slot);
		Texture::SetBlankSamplerState(slot);
	}

	slot = 2;
	if (normalMap != NULL)
	{
		normalMap->SetShaderResource(slot);
		normalMap->SetShaderSampler(slot);
	}
	else
	{
		Texture::SetBlankShaderResource(slot);
		Texture::SetBlankSamplerState(slot);
	}


	buffer->SetPSBuffer(1);
}

void Material::Clone(void ** clone)
{
	Material* material = new Material();
	material->name = this->name;

	if (this->shader != NULL)
		material->SetShader(shader);

	material->SetDiffuse(*this->GetDiffuse());
	material->SetSpecular(*this->GetSpecular());

	if (this->diffuseMap != NULL)
		material->SetDiffuseMap(this->diffuseMap->GetFile());

	if (this->specularMap != NULL)
		material->SetSpecularMap(this->specularMap->GetFile());

	if (this->normalMap != NULL)
		material->SetNormalMap(this->normalMap->GetFile());


	material->SetShininess(*this->GetShininess());

	*clone = material;
}