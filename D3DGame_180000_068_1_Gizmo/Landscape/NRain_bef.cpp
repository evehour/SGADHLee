#include "stdafx.h"
#include "NRain.h"

float random()
{
	return (float((double)rand() / ((double)(RAND_MAX)+(double)(1))));
}

NRain::NRain(ExecuteValues* values)
	: values(values)
	, firstFrame(false), time(0.0f)
{
	WindAnimation.clear();
	int time = 0;
	WindAnimation.push_back(WindValue(D3DXVECTOR3(-0.1, -0.5, 0), time));
	WindAnimation.push_back(WindValue(D3DXVECTOR3(-0.4, -0.5, 0.04), time += 10));
	WindAnimation.push_back(WindValue(D3DXVECTOR3(-0.2, -0.5, -0.4), time += 5));
	WindAnimation.push_back(WindValue(D3DXVECTOR3(0.0, -0.5, -0.02), time += 10));
	WindAnimation.push_back(WindValue(D3DXVECTOR3(0.0, -0.5, -0.02), time += 10));
	WindAnimation.push_back(WindValue(D3DXVECTOR3(0.1, -0.5, 0.4), time += 6));
	WindAnimation.push_back(WindValue(D3DXVECTOR3(-0.1, -0.5, 0), time += 5));

	//Create Vertex
	{
		D3DXVECTOR3 camPos;
		values->MainCamera->Position(&camPos);

		RainVertex vertices[3];// = new RainVertex[g_numRainVertices];
		for (int i = 0;i<g_numRainVertices;i++)
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
			raindrop.seed = D3DXVECTOR3(SeedX, SeedY, SeedZ);

			//add some random speed to the particles, to prevent all the particles from following exactly the same trajectory
			//additionally, random speeds in the vertical direction ensure that temporal aliasing is minimized
			float SpeedX = 40.0f*(random() / 20.0f);
			float SpeedZ = 40.0f*(random() / 20.0f);
			float SpeedY = 40.0f*(random() / 10.0f);
			raindrop.speed = D3DXVECTOR3(SpeedX, SpeedY, SpeedZ);

			//move the rain particles to a random positions in a cylinder above the camera
			float x = SeedX + camPos.x;
			float z = SeedZ + camPos.z;
			float y = SeedY + camPos.y;
			raindrop.pos = D3DXVECTOR3(x, y, z);

			//get an integer between 1 and 8 inclusive to decide which of the 8 types of rain textures the particle will use
			raindrop.Type = int(floor(random() * 8 + 1));

			//this number is used to randomly increase the brightness of some rain particles
			raindrop.random = 1;
			float randomIncrease = random();
			if (randomIncrease > 0.8)
				raindrop.random += randomIncrease;

			vertices[i] = raindrop;
		}

		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(RainVertex) * g_numRainVertices;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;
		data.SysMemPitch = sizeof(RainVertex);

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &g_pParticleStart);
		assert(SUCCEEDED(hr));


		desc.BindFlags |= D3D11_BIND_STREAM_OUTPUT;
		
		hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &g_pParticleDrawFrom);
		assert(SUCCEEDED(hr));

		hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &g_pParticleStreamTo);
		assert(SUCCEEDED(hr));

		//SAFE_DELETE_ARRAY(vertices);
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

	// Setting Buffers
	{
		worldBuffer = new WorldBuffer();
		psBuffer = new PSBuffer();
		globalCSBuffer = new GlobalCSShaderBuffer();
	}

	// Setting Shaders
	{
		advancedRainShader = new Shader(Shaders + L"050_AdvancedRain.hlsl", "VSAdvanceRain", "");
		D3D11_SO_DECLARATION_ENTRY pDecl[] =
		{
			// Stream, semantic name, semantic index, start component, component count, output slot
			{ 0, "POSITION", 0, 0, 3, 0 },
			{ 0, "SEED",     0, 0, 3, 0 },
			{ 0, "SPEED",    0, 0, 3, 0 },
			{ 0, "RAND",     0, 0, 1, 0 },
			{ 0, "TYPE",     0, 0, 1, 0 },
		};
		advancedRainShader->CreateGSWithSO(pDecl, sizeof(pDecl) / sizeof(D3D11_SO_DECLARATION_ENTRY), "gsStreamOut");

		renderParticle = new Shader(Shaders + L"050_AdvancedRain.hlsl", "VSPassThroughRain", "PSRenderRain");
		renderParticle->CreateGS("GSRenderRain");

		depthStencilState[0] = new DepthStencilState();
		depthStencilState[1] = new DepthStencilState();
		depthStencilState[1]->DepthEnable(false);
		depthStencilState[1]->DepthWriteEnable(D3D11_DEPTH_WRITE_MASK_ZERO);
	}

	// Preset Settings;
	{
		g_WindAmount = g_constWindAmount1;
	}
}

NRain::~NRain()
{
	for (UINT i = 0; i < 2; i++)
		SAFE_DELETE(depthStencilState[i]);

	SAFE_DELETE(textureArray);

	SAFE_RELEASE(g_pParticleStreamTo);
	SAFE_RELEASE(g_pParticleDrawFrom);
	SAFE_RELEASE(g_pParticleStart);
	
	SAFE_DELETE(worldBuffer);

	SAFE_DELETE(globalCSBuffer);
	SAFE_DELETE(renderParticle);
	SAFE_DELETE(advancedRainShader);
}

void NRain::Update()
{
	/*if (g_bMoveParticles)*/ psBuffer->Data.TimeCycle += 0.085;

	if (psBuffer->Data.TimeCycle >= 1.0f)
	{
		psBuffer->Data.TimeCycle = 0.0f;
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

		while (time > WindAnimation.at(upperFrame).time)
			upperFrame++;

		int lowerFrame = upperFrame - 1;
		float amount = float(time - WindAnimation.at(lowerFrame).time) / (WindAnimation.at(upperFrame).time - WindAnimation.at(lowerFrame).time);
		D3DXVECTOR3 interpolatedWind = WindAnimation.at(lowerFrame).windAmount + amount*(WindAnimation.at(upperFrame).windAmount - WindAnimation.at(lowerFrame).windAmount);

		//adjust the wind based on the current frame rate; the values were estimated for 40FPS
		interpolatedWind *= 40.0f / framesPerSecond;
		//lerp between the wind vector and just the vector pointing down depending on the amount of user chosen wind
		interpolatedWind = g_WindAmount*interpolatedWind + (1 - g_WindAmount)*(D3DXVECTOR3(0, -0.5, 0));

		globalCSBuffer->Data.g_TotalVel = interpolatedWind;
		time += 1.0f / framesPerSecond;
		if (time>lastTime)
			time = 0;
	}
}

void NRain::Render()
{
	UINT stride = sizeof(RainVertex);
	UINT offset = 0;

	globalCSBuffer->SetAllBuffer(8);

	ID3D11Buffer *pBuffers[1];
	{
		if (firstFrame)
			pBuffers[0] = g_pParticleStart;
		else
			pBuffers[0] = g_pParticleDrawFrom;

		D3D::GetDC()->IASetVertexBuffers(0, 1, pBuffers, &stride, &offset);
		D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		// Point to the correct output buffer
		pBuffers[0] = g_pParticleStreamTo;
		D3D::GetDC()->SOSetTargets(1, pBuffers, &offset);

		// draw
		depthStencilState[1]->OMSetDepthStencilState();

		advancedRainShader->Render();
		if (firstFrame)
			D3D::GetDC()->Draw(g_numRainVertices, 0);
		else
			D3D::GetDC()->DrawAuto();
		depthStencilState[0]->OMSetDepthStencilState();


		// Get back to normal
		pBuffers[0] = NULL;
		D3D::GetDC()->SOSetTargets(1, pBuffers, &offset);

		// Swap buffers
		//ID3D11Buffer* pTemp = g_pParticleDrawFrom;
		//g_pParticleDrawFrom = g_pParticleStreamTo;
		//g_pParticleStreamTo = pTemp;
		std::swap(g_pParticleDrawFrom, g_pParticleStreamTo);
	}

	{
		D3D::GetDC()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
		D3D::GetDC()->IASetVertexBuffers(0, 1, &g_pParticleDrawFrom, &stride, &offset);
		values->ViewProjection->SetGSBuffer(0);
		values->GlobalLight->SetGSBuffer(7);
		worldBuffer->SetGSBuffer(1);
		
		ID3D11ShaderResourceView* srv = textureArray->GetSRV();
		D3D::GetDC()->PSSetShaderResources(10, 1, &srv);

		renderParticle->Render();

		D3D::GetDC()->Draw(int(g_numRainVertices*g_DrawFraction), 0);
	}

	firstFrame = false;
}
