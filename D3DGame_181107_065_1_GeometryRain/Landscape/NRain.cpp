#include "stdafx.h"
#include "NRain.h"

float random()
{
	return (float((double)rand() / ((double)(RAND_MAX)+(double)(1))));
}

NRain::NRain(ExecuteValues* values)
	: values(values)
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

	{
		D3DXVECTOR3 camPos;
		values->MainCamera->Position(&camPos);

		RainVertex* vertices = new RainVertex[g_numRainVertices];
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

		SAFE_DELETE_ARRAY(vertices);
	}

	{
		textures = new Texture*[rainTextureCount];
		for (UINT i = 0; i < rainTextureCount; i++)
		{
			wstring form = String::Format(L"%S%.4d.dds", Textures + L"rainTextures/cv0_vPositive_", i);
			textures[i] = new Texture(form);
		}
	}
}

NRain::~NRain()
{
	for (UINT i = 0; i < rainTextureCount; i++)
	{
		SAFE_DELETE(textures[i]);
	}
	SAFE_DELETE_ARRAY(textures);

	SAFE_RELEASE(g_pParticleStreamTo);
	SAFE_RELEASE(g_pParticleDrawFrom);
	SAFE_RELEASE(g_pParticleStart);
}
