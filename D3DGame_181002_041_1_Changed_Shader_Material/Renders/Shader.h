#pragma once

class Shader
{
public:
	friend class Shaders;

public:
	Shader(wstring shaderFile, string vs = "VS", string ps = "PS");
	~Shader();

	void Render();

private:
	wstring file;
	string vs, ps;

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* inputLayout;
};

struct ShaderDesc
{
	wstring File;
	string Vs, Ps;

	ID3D11VertexShader* VertexShader;
	ID3D11PixelShader* PixelShader;
	ID3D11InputLayout* InputLayout;

	bool operator==(const ShaderDesc& desc)
	{
		bool b = true;
		b &= File == desc.File;
		b &= Vs == desc.Vs;
		b &= Ps == desc.Ps;

		return b;
	}
};

class Shaders
{
public:
	friend class Shader;

public:
	static void Delete();

private:
	static void Load(Shader* shader, wstring file, string vs, string ps);

	static void Compile(wstring file, string funcName, string profile, ID3DBlob** shaderBlob);
	static void CreateInputLayout(ID3DBlob* blob, ID3D11InputLayout** layout);

private:
	static vector<ShaderDesc> descs;
};