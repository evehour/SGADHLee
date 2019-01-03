#pragma once
#include "Interfaces/ICommand.h"

class CommandTerrain : public ICommand
{
public:
	CommandTerrain(ID3D11Texture2D* befTex2D, ID3D11Texture2D* afterTex2D, Shader* cs, ID3D11UnorderedAccessView* terrainUAV);
	~CommandTerrain();

	// ICommand을(를) 통해 상속됨
	int Execute() override;
	int Undo() override;

private:
	ID3D11Texture2D* beforeTexture;
	ID3D11Texture2D* afterTexture;

	ID3D11ShaderResourceView* beforeSRV;
	ID3D11ShaderResourceView* afterSRV;

	Shader* brushCShader;
	ID3D11UnorderedAccessView* terrainUAV;
};
