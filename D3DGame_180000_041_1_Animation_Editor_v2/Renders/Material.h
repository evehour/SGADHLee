#pragma once
#include "../Interfaces/ICloneable.h"

class Material : public ICloneable
{
private:
	class Buffer;

public:
	Material();
	Material(Shader* shader);
	~Material();

	void Name(wstring val) { name = val; }
	wstring Name() { return name; }

	Shader* GetShader() { return shader; }
	void SetShader(Shader* shader);


	D3DXCOLOR* GetDiffuse() { return &buffer->Data.Diffuse; }
	void SetDiffuse(D3DXCOLOR& color) { buffer->Data.Diffuse = color; }
	void SetDiffuse(float r, float g, float b, float a = 1.0f)
	{
		buffer->Data.Diffuse = D3DXCOLOR(r, g, b, a);
	}

	D3DXCOLOR* GetSpecular() { return &buffer->Data.Specular; }
	void SetSpecular(D3DXCOLOR& color) { buffer->Data.Specular = color; }
	void SetSpecular(float r, float g, float b, float a = 1.0f)
	{
		buffer->Data.Specular = D3DXCOLOR(r, g, b, a);
	}

	float* GetShininess() { return &buffer->Data.Shininess; }
	void SetShininess(float val) { buffer->Data.Shininess = val; }


	Texture* GetDiffuseMap() { return diffuseMap; }
	void SetDiffuseMap(string file, D3DX11_IMAGE_LOAD_INFO* info = NULL);
	void SetDiffuseMap(wstring file, D3DX11_IMAGE_LOAD_INFO* info = NULL);
	void SetDiffuseMapA(wstring file) { SetDiffuseMap(file); }

	Texture* GetSpecularMap() { return specularMap; }
	void SetSpecularMap(string file, D3DX11_IMAGE_LOAD_INFO* info = NULL);
	void SetSpecularMap(wstring file, D3DX11_IMAGE_LOAD_INFO* info = NULL);
	void SetSpecularMapA(wstring file) { SetSpecularMap(file); }

	Texture* GetNormalMap() { return normalMap; }
	void SetNormalMap(string file, D3DX11_IMAGE_LOAD_INFO* info = NULL);
	void SetNormalMap(wstring file, D3DX11_IMAGE_LOAD_INFO* info = NULL);
	void SetNormalMapA(wstring file) { SetNormalMap(file); }

	Texture* GetDetailMap() { return detailMap; }
	void SetDetailMap(string file, D3DX11_IMAGE_LOAD_INFO* info = NULL);
	void SetDetailMap(wstring file, D3DX11_IMAGE_LOAD_INFO* info = NULL);
	void SetDetailMapA(wstring file) { SetDetailMap(file); }

	void PSSetBuffer();

public:
	void Clone(void** clone);

private:
	wstring name;
	Buffer* buffer;

	Shader* shader;

	Texture* diffuseMap;
	Texture* specularMap;
	Texture* normalMap;
	Texture* detailMap;

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Struct))
		{
			Data.Diffuse = D3DXCOLOR(1, 1, 1, 1);
			Data.Specular = D3DXCOLOR(1, 1, 1, 1);

			Data.Shininess = 1;
		}

		struct Struct
		{
			D3DXCOLOR Diffuse;
			D3DXCOLOR Specular;

			float Shininess;

			float Padding[3];
		} Data;
	};
};