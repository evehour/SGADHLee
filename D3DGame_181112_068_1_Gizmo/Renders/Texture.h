#pragma once

class Texture
{
public:
	friend class Textures;

public:
	static void SetShaderResources(UINT slot, UINT count, Texture** textures);
	static void SetBlankShaderResource(UINT slot);
	static void SetBlankSamplerState(UINT slot);

	static void SaveFile(wstring file, ID3D11Texture2D* src);
	static D3D11_TEXTURE2D_DESC ReadPixels(ID3D11Texture2D* src, DXGI_FORMAT readFormat, vector<D3DXCOLOR>* pixels);
	static void WritePixels(ID3D11Texture2D* src, DXGI_FORMAT writeFormat, vector<D3DXCOLOR>& pixels, bool isInteger = false);

public:
	Texture(wstring file, D3DX11_IMAGE_LOAD_INFO* loadInfo = NULL);
	~Texture();


	wstring GetFile() { return file; }

	UINT GetWidth() { return metaData.width; }
	UINT GetHeight() { return metaData.height; }

	D3D11_TEXTURE2D_DESC ReadPixels(DXGI_FORMAT readFormat, vector<D3DXCOLOR>* pixels);
	void WritePixels(DXGI_FORMAT writeFormat, vector<D3DXCOLOR>& pixels, bool isInteger = false);
	void SaveFile(wstring file);

	void GetImageInfo(DirectX::TexMetadata* data)
	{
		*data = metaData;
	}

	void SetSamplerState(class SamplerState* state);

	ID3D11ShaderResourceView* GetView() { return view; }

	void SetShaderResource(UINT slot);
	void SetShaderSampler(UINT slot);

private:
	wstring file;

	DirectX::TexMetadata metaData;
	ID3D11ShaderResourceView* view;
	
	class SamplerState* samplerState;
};

struct TextureDesc
{
	wstring file;
	UINT width, height;
	DirectX::TexMetadata metaData;
	ID3D11ShaderResourceView* view;

	bool operator==(const TextureDesc& desc)
	{
		bool b = true;
		b &= file == desc.file;
		b &= width == desc.width;
		b &= height == desc.height;

		return b;
	}
};

class Textures
{
public:
	friend class Texture;

public:
	static void Create();
	static void Delete();

private:
	static void Load(Texture* texture, D3DX11_IMAGE_LOAD_INFO* loadInfo = NULL);

private:
	static vector<TextureDesc> descs;
};

class TextureArray
{
public:
	TextureArray(vector<wstring>& names, UINT width = 256, UINT height = 256, UINT mipLevels = 1);
	~TextureArray();

	ID3D11ShaderResourceView* GetSRV() { return srv; }
	ID3D11ShaderResourceView* GetView() { return srv; }

private:
	vector<ID3D11Texture2D *> CreateTextures(vector<wstring>& names, UINT width, UINT height, UINT mipLevels);

private:
	ID3D11ShaderResourceView* srv;
};