#include "stdafx.h"
#include "NRain.h"

#include <iostream>
#include <fstream>

float random()
{
	return (float((double)rand() / ((double)(RAND_MAX)+(double)(1))));
}

NRain::NRain(ExecuteValues* values)
	: values(values)
	, bFirstRender(true)
	, g_time(0.0f)
{
	{
		fogTexture = NULL;
		fogSRV = NULL;
		
		HRESULT hr;
		hr = GenFogTable("./F_512_data.csv", 512, 512, &fogTexture, &fogSRV);
		if (FAILED(hr))
		{
			SAFE_RELEASE(fogTexture);
			SAFE_RELEASE(fogSRV);
			assert(false);
		}
	}

	WindAnimation.clear();

	int time = 0;
	WindAnimation.push_back(WindValue(D3DXVECTOR3(-0.1f, -0.5f, 0), time));
	WindAnimation.push_back(WindValue(D3DXVECTOR3(-0.4f, -0.5f, 0.04f), time += 10));
	WindAnimation.push_back(WindValue(D3DXVECTOR3(-0.2f, -0.5f, -0.4f), time += 5));
	WindAnimation.push_back(WindValue(D3DXVECTOR3(0.0f, -0.5f, -0.02f), time += 10));
	WindAnimation.push_back(WindValue(D3DXVECTOR3(0.0f, -0.5f, -0.02f), time += 10));
	WindAnimation.push_back(WindValue(D3DXVECTOR3(0.1f, -0.5f, 0.4f), time += 6));
	WindAnimation.push_back(WindValue(D3DXVECTOR3(-0.1f, -0.5f, 0), time += 5));

	{
		RainVertex* vertices = new RainVertex[g_numRainVertices];
		//RainVertex vertices[15];
		D3DXVECTOR3 camPos;

		values->MainCamera->Position(&camPos);

		for (int i = 0;i < g_numRainVertices;i++)
		{
			RainVertex raindrop;
			//use rejection sampling to generate random points inside a circle of radius 1 centered at 0,0
			float SeedX;
			float SeedZ;
			bool pointIsInside = false;
			while (!pointIsInside)
			{
				SeedX = random() - 0.5f;
				SeedZ = random() - 0.5f;
				if (sqrt(SeedX*SeedX + SeedZ*SeedZ) <= 0.5f)
					pointIsInside = true;
			}
			//save these random locations for reinitializing rain particles that have fallen out of bounds
			SeedX *= g_radiusRange;
			SeedZ *= g_radiusRange;
			float SeedY = random()*g_heightRange;
			raindrop.Seed = D3DXVECTOR3(SeedX, SeedY, SeedZ);

			//add some random speed to the particles, to prevent all the particles from following exactly the same trajectory
			//additionally, random speeds in the vertical direction ensure that temporal aliasing is minimized
			float SpeedX = 40.0f*(random() / 20.0f);
			float SpeedZ = 40.0f*(random() / 20.0f);
			float SpeedY = 40.0f*(random() / 10.0f);
			raindrop.Speed = D3DXVECTOR3(SpeedX, SpeedY, SpeedZ);

			//move the rain particles to a random positions in a cylinder above the camera
			float x = SeedX + camPos.x;
			float z = SeedZ + camPos.z;
			float y = SeedY + camPos.y;
			raindrop.Position = D3DXVECTOR3(x, y, z);

			//get an integer between 1 and 8 inclusive to decide which of the 8 types of rain textures the particle will use
			raindrop.Type = int(floor(random() * 8 + 1));

			//this number is used to randomly increase the brightness of some rain particles
			//raindrop.random = 1;
			float randomIncrease = random();
			raindrop.Random = 0.0f;
			if (randomIncrease > 0.8)
				raindrop.Random += randomIncrease;

			vertices[i] = raindrop;
		}

		D3DXVECTOR4 vert = D3DXVECTOR4(vertices[0].Position.x, vertices[0].Position.y, vertices[0].Position.z, 1);
		D3DXVECTOR4 dVert;
		D3DXMATRIX view, proj, matF;
		values->MainCamera->Matrix(&view);
		values->Perspective->GetMatrix(&proj);
		matF = view * proj;
		D3DXVec4Transform(&dVert, &vert, &matF);

		float x = dVert.x / dVert.w;
		float y = dVert.y / dVert.w;
		float z = dVert.z / dVert.w;


		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(RainVertex) * g_numRainVertices;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;
		data.SysMemPitch = sizeof(RainVertex);

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &initBuffer);
		assert(SUCCEEDED(hr));

		//desc.ByteWidth = sizeof(VertexColor) * 3 * 3;
		desc.BindFlags |= D3D11_BIND_STREAM_OUTPUT;

		hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &drawVBuffer);
		assert(SUCCEEDED(hr));

		hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &SOVBuffer);
		assert(SUCCEEDED(hr));

		SAFE_DELETE(vertices);
	}

	{
		// Stream Output의 대상이 되는 GS는 아래와 같이 리플렉션이 아니라 자신이
		// 직접 넣는 것이기 때문에 D3DXVECTOR3를 주므로 크기를 맞춰줘야 한다.
		shader1 = new Shader(Shaders + L"050_AdvancedRain.hlsl", "VS", "");
		D3D11_SO_DECLARATION_ENTRY pDecl[] =
		{
			{ 0, "POSITION", 0, 0, 3, 0 },
			{ 0, "SEED", 0, 0, 3, 0 },
			{ 0, "SPEED", 0, 0, 3, 0 },
			{ 0, "RANDOM", 0, 0, 1, 0 },
			{ 0, "TYPE", 0, 0, 1, 0 },
		};
		shader1->CreateGSWithSO(
			pDecl,
			sizeof(pDecl) / sizeof(D3D11_SO_DECLARATION_ENTRY),
			"gsStreamOut"
		);

		shader2 = new Shader(Shaders + L"050_AdvancedRain.hlsl", "VSPassThrough", "PS");
		shader2->CreateGS("GS");

		// 스트림에만 출력을 하고 랜더링 하지 않는 작업에서는
		// 깊이버퍼(마스크는 제로)를 꺼야 한다.
		depthStencilState[0] = new DepthStencilState();
		depthStencilState[1] = new DepthStencilState();
		depthStencilState[1]->DepthEnable(false);
		depthStencilState[1]->DepthWriteEnable(D3D11_DEPTH_WRITE_MASK_ZERO);

		rasterizerState[0] = new RasterizerState();
		rasterizerState[1] = new RasterizerState();
		//rasterizerState[1]->CullMode(D3D11_CULL_NONE);
		rasterizerState[1]->CullMode(D3D11_CULL_FRONT);

		blendState[0] = new BlendState();
		blendState[1] = new BlendState();
		blendState[1]->BlendEnable(true);
	}

	// Setting Textures
	{
		vector<wstring> tFiles;
		for (UINT i = 0; i < rainTextureCount; i++)
		{
			string sform;
			sform = String::Format("%s%.4d.dds", "rainTextures/cv0_vPositive_", i);
			tFiles.push_back(Textures + String::ToWString(sform));
		}
		textureArray = new TextureArray(tFiles, 16, 526);
	}

	{
		globalCSBuffer = new GlobalCSShaderBuffer();
		globalCSBuffer->Data.g_TotalVel = WindAnimation.at(0).windAmount;

		psBuffer = new PSBuffer();

		worldBuffer = new WorldBuffer();
	}
}

NRain::~NRain()
{
	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(psBuffer);
	SAFE_DELETE(globalCSBuffer);

	for (UINT i = 0; i < 2; i++)
	{
		SAFE_DELETE(rasterizerState[i]);
		SAFE_DELETE(depthStencilState[i]);
		SAFE_DELETE(blendState[i]);
	}

	SAFE_DELETE(textureArray);

	SAFE_DELETE(shader2);
	SAFE_DELETE(shader1);

	SAFE_RELEASE(initBuffer);
	SAFE_RELEASE(drawVBuffer);
	SAFE_RELEASE(SOVBuffer);

	SAFE_RELEASE(fogSRV);
	SAFE_RELEASE(fogTexture);
}

void NRain::Update()
{
	//psBuffer->Data.TimeCycle += 0.085f;
	psBuffer->Data.TimeCycle += 5.0f * Time::Delta();

	if (psBuffer->Data.TimeCycle >= 1.0f)
	{
		psBuffer->Data.TimeCycle -= 1.0f;
		psBuffer->Data.g_rainSplashesXDisplaceShaderVariable = random() * 2.0f;
		psBuffer->Data.g_rainSplashesYDisplaceShaderVariable = random() * 2.0f;
	}

	//if (g_bMoveParticles)
	{
		//estimate a value of wind at the given time frame
		//we use piecewise linear interpolation between an array of key frames of wind values

		int lastTime = WindAnimation.back().time;
		int upperFrame = 1;
		float framesPerSecond = Time::Get()->FPS();
		globalCSBuffer->Data.g_FrameRate = framesPerSecond;

		while (g_time > WindAnimation.at(upperFrame).time)
			upperFrame++;

		int lowerFrame = upperFrame - 1;
		float amount = float(g_time - WindAnimation.at(lowerFrame).time) / (WindAnimation.at(upperFrame).time - WindAnimation.at(lowerFrame).time);
		D3DXVECTOR3 interpolatedWind = WindAnimation.at(lowerFrame).windAmount + amount*(WindAnimation.at(upperFrame).windAmount - WindAnimation.at(lowerFrame).windAmount);

		//adjust the wind based on the current frame rate; the values were estimated for 40FPS
		interpolatedWind *= 40.0f / framesPerSecond;

		//lerp between the wind vector and just the vector pointing down depending on the amount of user chosen wind
		interpolatedWind = g_WindAmount * interpolatedWind + (1.0f - g_WindAmount) * (D3DXVECTOR3(0, -0.5f, 0));

		globalCSBuffer->Data.g_TotalVel = interpolatedWind;
		g_time += 1.0f / framesPerSecond;
		if (g_time > lastTime)
			g_time = 0;
	}
}

void NRain::Render()
{
	UINT stride = sizeof(RainVertex);
	UINT offset = 0;

	ID3D11Buffer *pBuffers[1];
	if (bFirstRender)
		pBuffers[0] = initBuffer;
	else
		pBuffers[0] = drawVBuffer;

	D3D::GetDC()->IASetVertexBuffers(0, 1, pBuffers, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	worldBuffer->SetAllBuffer(1);
	globalCSBuffer->SetAllBuffer(8);

	// Point to the correct output buffer
	pBuffers[0] = SOVBuffer;
	D3D::GetDC()->SOSetTargets(1, pBuffers, &offset);

	depthStencilState[1]->OMSetDepthStencilState();
	shader1->Render();

	if (bFirstRender)
		D3D::GetDC()->Draw(g_numRainVertices, 0);
	else
	{
#ifdef RAIN_USING_DRAWAUTO
		D3D::GetDC()->DrawAuto();
#else
		D3D::GetDC()->Draw(g_numRainVertices, 0);
#endif
	}

	depthStencilState[0]->OMSetDepthStencilState();

	ID3D11Buffer *bufferArray[1] = { 0 };
	D3D::GetDC()->SOSetTargets(1, bufferArray, &offset);

	std::swap(drawVBuffer, SOVBuffer);
	values->ViewProjection->SetGSBuffer(0);
	psBuffer->SetPSBuffer(9);

	ID3D11ShaderResourceView* srv = textureArray->GetSRV();
	D3D::GetDC()->PSSetShaderResources(10, 1, &srv);
	D3D::GetDC()->PSSetShaderResources(11, 1, &fogSRV);
	
	rasterizerState[1]->RSSetState();
	blendState[1]->OMSetBlendState();

	shader2->Render();

#ifdef RAIN_USING_DRAWAUTO
	D3D::GetDC()->DrawAuto();
#else
	D3D::GetDC()->Draw(g_numRainVertices, 0);
#endif

	D3D::GetDC()->GSSetShader(NULL, NULL, 0);

	blendState[0]->OMSetBlendState();
	rasterizerState[0]->RSSetState();

	bFirstRender = false;


	//ImGui Part
	{
		ImGui::SliderFloat("Fog::Beta", &psBuffer->Data.Beta.x, 0.0f, 1.0f);
		psBuffer->Data.Beta = D3DXVECTOR3(psBuffer->Data.Beta.x, psBuffer->Data.Beta.x, psBuffer->Data.Beta.x);
	}
}

HRESULT NRain::GenFogTable(string fileName, int xRes, int yRes, ID3D11Texture2D** fogTexture2D, ID3D11ShaderResourceView** fogShaderResourceView)
{
	HRESULT hr;
	ifstream infile(fileName, ios::in);

	if (infile.is_open())
	{
		float* data = new float[xRes*yRes];
		int index = 0;
		char tempc;
		for (int j = 0;j<yRes;j++)
		{
			for (int i = 0;i<xRes - 1;i++)
				infile >> data[index++] >> tempc;
			infile >> data[index++];

		}

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.SysMemPitch = sizeof(float) * xRes;
		InitData.pSysMem = data;

		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
		texDesc.Width = xRes;
		texDesc.Height = yRes;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R32_FLOAT;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		hr = D3D::GetDevice()->CreateTexture2D(&texDesc, &InitData, fogTexture2D);
		assert(SUCCEEDED(hr));

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = texDesc.Format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;

		hr = D3D::GetDevice()->CreateShaderResourceView(*fogTexture2D, &SRVDesc, fogShaderResourceView);
		assert(SUCCEEDED(hr));
		delete[] data;
	}
	else
		hr = S_FALSE;

	return hr;
}
