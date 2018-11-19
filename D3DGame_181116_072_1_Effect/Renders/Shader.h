#pragma once

class Shader
{
public:
	Shader(wstring file);
	~Shader();

	wstring GetFile() { return file; }

	ID3DX11Effect* Effect() { return effect; }
	
	void IASetInputLayout(UINT technique, UINT pass);
	void Apply(UINT technique, UINT pass);

	ID3DX11EffectVariable* Variable(string name);
	ID3DX11EffectScalarVariable* AsScalar(string name);
	ID3DX11EffectVectorVariable* AsVector(string name);
	ID3DX11EffectMatrixVariable* AsMatrix(string name);
	ID3DX11EffectStringVariable* AsString(string name);
	ID3DX11EffectShaderResourceVariable* AsShaderResource(string name);
	ID3DX11EffectRenderTargetViewVariable* AsRenderTargetView(string name);
	ID3DX11EffectDepthStencilViewVariable* AsDepthStencilView(string name);
	ID3DX11EffectConstantBuffer* AsConstantBuffer(string name);
	ID3DX11EffectShaderVariable* AsShader(string name);
	ID3DX11EffectBlendVariable* AsBlend(string name);
	ID3DX11EffectDepthStencilVariable* AsDepthStencil(string name);
	ID3DX11EffectRasterizerVariable* AsRasterizer(string name);
	ID3DX11EffectSamplerVariable* AsSampler(string name);

private:
	void CreateEffect();
	ID3D11InputLayout* CreateInputLayout(ID3DBlob* fxBlob, D3DX11_EFFECT_SHADER_DESC* effectVsDesc, vector<D3D11_SIGNATURE_PARAMETER_DESC>& params);

private:
	wstring file;

	ID3DX11Effect* effect;
	D3DX11_EFFECT_DESC effectDesc;

private:
	struct EffectBuffer
	{
		UINT index;
		ID3DX11EffectConstantBuffer* Buffer;
		D3DX11_EFFECT_VARIABLE_DESC Desc;

		ID3DX11EffectVariable* Variable(string name)
		{
			return Buffer->GetAnnotationByName(name.c_str());
		}

	};
	map<string, EffectBuffer> cBufferMap;

	struct EffectVariable
	{
		UINT index;
		ID3DX11EffectVariable* Variable;

	};
	map<string, EffectVariable> variableMap;

private:
	struct Pass
	{
		wstring Name;
		ID3DX11EffectPass* IPass;
		D3DX11_PASS_DESC Desc;

		ID3D11InputLayout* InputLayout;
		D3DX11_PASS_SHADER_DESC PassVsDesc;
		D3DX11_EFFECT_SHADER_DESC EffectVsDesc;
		vector<D3D11_SIGNATURE_PARAMETER_DESC> SignatureDescs;

		void IASetInputLayout();
		void Apply();
	};

	struct Technique
	{
		wstring Name;
		D3DX11_TECHNIQUE_DESC Desc;
		ID3DX11EffectTechnique* ITechnique;

		vector<Pass> Passes;

		void IASetInputLayout(UINT pass);
		void Apply(UINT pass);
	};
	vector<Technique> techniques;
};