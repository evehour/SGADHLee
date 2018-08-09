#pragma once
#include "stdafx.h"

namespace Fbx
{
	struct FbxMaterial
	{
		string Name;
		
		D3DXCOLOR Diffuse;

		string DiffuseFile;
	};
}