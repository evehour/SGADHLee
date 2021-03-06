#include "stdafx.h"
#include "Sky.h"

#include "./Viewer/Camera.h"

Sky::Sky()
	: realTime(false), theta(0.0f), phi(0)
	, radius(10), slices(32), stacks(16)
	, timeFactor(0.0f)
{
	mieTarget = new RenderTarget(128, 64);
	rayleighTarget = new RenderTarget(128, 64);

	shader = new Shader(Shaders + L"054_Scattering.fx");

	//scatterShader = new Shader(Shaders + L"044_Scattering.hlsl", "VS_Scattering", "PS_Scattering");
	//targetShader = new Shader(Shaders + L"044_Scattering.hlsl", "VS_Target", "PS_Target");
	//moonShader = new Shader(Shaders + L"044_Scattering.hlsl", "VS_Moon", "PS_Moon");
	//cloudShader = new Shader(Shaders + L"044_Scattering.hlsl", "VS_Cloud", "PS_Cloud");

	{
		SampleCount = 20;

		WaveLength = D3DXVECTOR3(0.65f, 0.57f, 0.475f);
		InvWaveLength.x = 1.0f / powf(WaveLength.x, 4.0f);
		InvWaveLength.y = 1.0f / powf(WaveLength.y, 4.0f);
		InvWaveLength.z = 1.0f / powf(WaveLength.z, 4.0f);

		WaveLengthMie.x = powf(WaveLength.x, -0.84f);
		WaveLengthMie.y = powf(WaveLength.y, -0.84f);
		WaveLengthMie.z = powf(WaveLength.z, -0.84f);

		shader->AsScalar("SampleCount")->SetInt(SampleCount);
		shader->AsVector("WaveLength")->SetFloatVector(WaveLength);
		shader->AsVector("InvWaveLength")->SetFloatVector(InvWaveLength);
		shader->AsVector("WaveLengthMie")->SetFloatVector(WaveLengthMie);
	}

	{
		StarIntensity = 0.0f;
		MoonAlpha = 0.0f;
	}

	GenerateSphere();
	GenerateQuad();

	//alphaBlend[1]->BlendEnable(true);
	////alphaBlend[1]->AlphaToCoverageEnable(true);
	//alphaBlend[1]->DestBlend(D3D11_BLEND_ONE);
	////alphaBlend[1]->DestBlend(D3D11_BLEND_INV_SRC_ALPHA);
	//alphaBlend[1]->SrcBlend(D3D11_BLEND_SRC_ALPHA);
	//alphaBlend[1]->BlendOP(D3D11_BLEND_OP_ADD);

	starField = new Texture(Textures + L"starfield.png");
	shader->AsShaderResource("StarfieldMap")->SetResource(starField->SRV());

	moon = new Texture(Textures + L"moon.tga");
	shader->AsShaderResource("MoonMap")->SetResource(moon->SRV());

	moonGlow = new Texture(Textures + L"moonglow.tga");
	shader->AsShaderResource("MoonGlowMap")->SetResource(moonGlow->SRV());

	rayleigh2D = new Render2D();
	rayleigh2D->Position(0, 100);
	rayleigh2D->Scale(200, 100);

	mie2D = new Render2D();
	mie2D->Position(0, 0);
	mie2D->Scale(200, 100);

	noise2D = new Render2D();
	noise2D->Position(1024, 0);
	noise2D->Scale(256, 256);


	int perm[] = { 151,160,137,91,90,15,
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};

	int gradValues[] = {
		1,1,0,
		-1,1,0, 1,-1,0,
		-1,-1,0, 1,0,1,
		-1,0,1, 1,0,-1,
		-1,0,-1, 0,1,1,
		0,-1,1, 0,1,-1,
		0,-1,-1, 1,1,0,
		0,-1,1, -1,1,0,
		0,-1,-1
	};
	cloud = new Texture(Textures + L"InitColorMap256.png");

	vector<D3DXCOLOR> pixels;
	for (int i = 0; i<256; i++)
	{
		for (int j = 0; j<256; j++)
		{
			//int offset = (i * 256 + j);
			int value = perm[(j + perm[i]) & 0xFF];
			D3DXCOLOR color;
			color.r = (float)(gradValues[value & 0x0F] * 64 + 64);
			color.g = (float)(gradValues[value & 0x0F + 1] * 64 + 64);
			color.b = (float)(gradValues[value & 0x0F + 2] * 64 + 64);
			color.a = (float)value;
			pixels.push_back(color);
		}
	}
	cloud->WritePixels(DXGI_FORMAT_R8G8B8A8_UNORM, pixels, true);

	shader->AsShaderResource("CloudMap")->SetResource(cloud->SRV());
}

Sky::~Sky()
{
	SAFE_RELEASE(quadBuffer);
	SAFE_DELETE_ARRAY(quadVertices);

	// Texture
	SAFE_DELETE(cloud);
	SAFE_DELETE(moonGlow);
	SAFE_DELETE(moon);
	SAFE_DELETE(starField);

	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE(shader);

	SAFE_DELETE(rayleighTarget);
	SAFE_DELETE(mieTarget);

	SAFE_DELETE(noise2D);
	SAFE_DELETE(mie2D);
	SAFE_DELETE(rayleigh2D);
}

void Sky::Update()
{
	D3DXMATRIX V;
	D3DXVECTOR3 pos;
	Context::Get()->GetMainCamera()->Position(&pos);
	D3DXMatrixTranslation(&V, pos.x, pos.y, pos.z);

	theta += Time::Delta() * timeFactor;
	if (theta > Math::PI * 2.0f) theta -= Math::PI * 2.0f;
	
	lightDirection = GetDirection();
	Context::Get()->GetGlobalLight()->Direction = lightDirection;
	Context::Get()->ChangeGlobalLight();

	StarIntensity = lightDirection.y;

	world = V;
}

void Sky::PreRender()
{
	if (prevTheta == theta && prevPhi == phi) return;

	mieTarget->Set();
	rayleighTarget->Set();

	ID3D11RenderTargetView* rtvs[2];
	rtvs[0] = rayleighTarget->RTV();
	rtvs[1] = mieTarget->RTV();

	ID3D11DepthStencilView* dsv;
	dsv = rayleighTarget->DSV();

	D3D::Get()->SetRenderTargets(2, rtvs, dsv);

	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &quadBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->Draw(0, 1, 6);
}

void Sky::Render()
{
	D3D::Get()->SetRenderTarget();
	D3DXVECTOR3 camPos;
	Context::Get()->GetMainCamera()->Position(&camPos);

	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->AsShaderResource("RayleighMap")->SetResource(rayleighTarget->SRV());
	shader->AsShaderResource("MieMap")->SetResource(mieTarget->SRV());
	
	shader->AsScalar("StarIntensity")->SetFloat(StarIntensity);

	shader->AsMatrix("World")->SetMatrix(world);
	shader->DrawIndexed(0, 0, indexCount);

	// Moon
	{
		D3DXMATRIX sc, rX, rY, dist, cam, matF;
		D3DXMatrixScaling(&sc, 50, 50, 1);
		D3DXMatrixRotationX(&rX, -theta - (Math::PI * 0.5f));
		D3DXMatrixRotationY(&rY, phi - (Math::PI * 0.5f));
		D3DXMatrixTranslation(&cam, camPos.x, camPos.y, camPos.z);

		if (theta < Math::PI * 0.5f || theta > Math::PI * 1.5f)
			MoonAlpha = (float)abs(sinf(theta + (float)Math::PI / 2.0f));
		else
			MoonAlpha = 0.0f;
		shader->AsScalar("MoonAlpha")->SetFloat(MoonAlpha);

		UINT stride = sizeof(VertexTexture);
		UINT offset = 0;

		D3DXMATRIX helpMat = sc*rX*rY;
		{
			// Moon
			D3D::GetDC()->IASetVertexBuffers(0, 1, &quadBuffer, &stride, &offset);
			D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			D3DXMatrixTranslation(&dist,
				lightDirection.x * 815.0f,
				lightDirection.y * 815.0f,
				lightDirection.z * 815.0f);
			matF = helpMat*dist*cam;

			shader->AsMatrix("World")->SetMatrix(matF);

			shader->Draw(0, 2, 6);
		}

		{
			D3DXMatrixScaling(&sc, 150, 150, 1);
			D3DXMatrixTranslation(&dist,
				lightDirection.x * 805.0f,
				lightDirection.y * 805.0f,
				lightDirection.z * 805.0f);
			matF = helpMat*dist*cam;

			shader->AsMatrix("World")->SetMatrix(matF);

			shader->Draw(0, 3, 6);
		}
	}

	// Cloud
	{
		UINT stride = sizeof(VertexTexture);
		UINT offset = 0;
		D3D::GetDC()->IASetVertexBuffers(0, 1, &quadBuffer, &stride, &offset);
		D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		D3DXMATRIX sc, rX, rY, dist, cam, matF;

		D3DXMatrixScaling(&sc, 900, 900, 1);
		D3DXMatrixTranslation(&dist, 0, 0, -200.0f);
		D3DXMatrixRotationX(&rX, Math::PI * 0.5f);
		D3DXMatrixTranslation(&cam, camPos.x, camPos.y, camPos.z);
		matF = sc * dist * rX * cam;

		shader->AsMatrix("World")->SetMatrix(matF);

		shader->AsScalar("NumTiles")->SetFloat(16.25f);
		shader->AsScalar("NoiseTime")->SetFloat(Time::Get()->Running() / 8.0f);
		shader->Draw(0, 4, 6);

		//ImGui::Text("%f, %f, %f", matF._41, matF._42, matF._43);
		//ImGui::Text("%f", theta);
		//ImGui::Text("%f", Time::Get()->Running() / 64.0f);
		//ImGui::SliderFloat("Time Factor", &timeFactor, -1.0f, 1.0f, "%.5f", 0.0001f);
		ImGui::DragFloat("Time Factor", &timeFactor, 0.001f, -1.0f, 1.0f);
	}

	rayleigh2D->SRV(rayleighTarget->SRV());
	rayleigh2D->Update();
	rayleigh2D->Render();

	mie2D->SRV(mieTarget->SRV());
	mie2D->Update();
	mie2D->Render();

	noise2D->SRV(cloud->SRV());
	noise2D->Update();
	noise2D->Render();

	ImGui::SliderFloat("Theta", &theta, 0.0f, Math::PI * 2.0f, "%.5f");
}

void Sky::PostRender()
{
}

void Sky::ResizeScreen()
{
}

void Sky::GenerateSphere()
{
#if false
	UINT radius = 10, slices = 32, stacks = 16;

	float sliceStep = (Math::PI * 2) / slices;
	float stackStep = (Math::PI) / stacks;

	///Create Vertex Data 
	vertexCount = (slices + 1) * (stacks + 1);
	VertexTexture* vertices = new VertexTexture[vertexCount];
	{
		float stackAngle = 0.0f; // - (Math::PI / 2);
		for (UINT y = 0; y <= stacks; y++) // 위도, altitude
		{
			float sliceAngle = 0.0f;
			for (UINT xz = 0; xz <= slices; xz++) // 경도, longitude
			{
				UINT index = (slices + 1) * y + xz;

				vertices[index].Position.x = (float)(radius * cosf(stackAngle) * cosf(sliceAngle));
				vertices[index].Position.y = (float)(radius * sinf(stackAngle));
				vertices[index].Position.z = (float)(radius * cosf(stackAngle) * sinf(sliceAngle));

				vertices[index].Uv.y = (1 / (float)slices) * xz;
				vertices[index].Uv.x = (1 / (float)stacks) * y;

				sliceAngle += sliceStep;
			}
			stackAngle += stackStep;
		}
	}

	///Create Index Data 
	indexCount = slices * stacks * 6;
	UINT* indices = new UINT[indexCount];
	{
		UINT index = 0;
		for (UINT y = 0; y < stacks; y++)
		{
			for (UINT x = 0; x < slices; x++)
			{
				indices[index + 0] = (slices + 1) * y + x; //0
				indices[index + 1] = (slices + 1) * (y + 1) + x; //1
				indices[index + 2] = (slices + 1) * y + (x + 1); //2

				indices[index + 3] = (slices + 1) * y + (x + 1); //2
				indices[index + 4] = (slices + 1) * (y + 1) + x; //1
				indices[index + 5] = (slices + 1) * (y + 1) + (x + 1); //3

				index += 6;
			}
		}//for(z)
	}
#else
	UINT latitude = 32 / 2; // 위도
	UINT longitude = 32; // 경도

	vertexCount = longitude * latitude * 2;
	indexCount = (longitude - 1) * (latitude - 1) * 2 * 8;

	VertexTexture* vertices = new VertexTexture[vertexCount];

	UINT index = 0;
	for (UINT i = 0; i < longitude; i++)
	{
		float xz = 100.0f * (i / (longitude - 1.0f)) * Math::PI / 180.0f;

		for (UINT j = 0; j < latitude; j++)
		{
			float y = Math::PI * j / (latitude - 1);

			vertices[index].Position.x = sinf(xz) * cosf(y);
			vertices[index].Position.y = cosf(xz);
			vertices[index].Position.z = sinf(xz) * sinf(y);
			vertices[index].Position *= 10.0f; // 크기를 키우려고 임의의 값 곱한거

			vertices[index].Uv.x = 0.5f / (float)longitude + i / (float)longitude;
			vertices[index].Uv.y = 0.5f / (float)latitude + j / (float)latitude;

			index++;
		} // for(j)
	}  // for(i)

	for (UINT i = 0; i < longitude; i++)
	{
		float xz = 100.0f * (i / (longitude - 1.0f)) * Math::PI / 180.0f;

		for (UINT j = 0; j < latitude; j++)
		{
			float y = (Math::PI * 2.0f) - (Math::PI * j / (latitude - 1));

			vertices[index].Position.x = sinf(xz) * cosf(y);
			vertices[index].Position.y = cosf(xz);
			vertices[index].Position.z = sinf(xz) * sinf(y);
			vertices[index].Position *= 10.0f; // 크기를 키우려고 임의의 값 곱한거

			vertices[index].Uv.x = 0.5f / (float)longitude + i / (float)longitude;
			vertices[index].Uv.y = 0.5f / (float)latitude + j / (float)latitude;

			index++;
		} // for(j)
	}  // for(i)


	index = 0;
	UINT* indices = new UINT[indexCount * 3];

	for (UINT i = 0; i < longitude - 1; i++)
	{
		for (UINT j = 0; j < latitude - 1; j++)
		{
			indices[index++] = i * latitude + j;
			indices[index++] = (i + 1) * latitude + j;
			indices[index++] = (i + 1) * latitude + (j + 1);

			indices[index++] = (i + 1) * latitude + (j + 1);
			indices[index++] = i * latitude + (j + 1);
			indices[index++] = i * latitude + j;
		}
	}

	UINT offset = latitude * longitude;
	for (UINT i = 0; i < longitude - 1; i++)
	{
		for (UINT j = 0; j < latitude - 1; j++)
		{
			indices[index++] = offset + i * latitude + j;
			indices[index++] = offset + (i + 1) * latitude + (j + 1);
			indices[index++] = offset + (i + 1) * latitude + j;

			indices[index++] = offset + i * latitude + (j + 1);
			indices[index++] = offset + (i + 1) * latitude + (j + 1);
			indices[index++] = offset + i * latitude + j;
		}
	}
#endif

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = indices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);
}

void Sky::GenerateQuad()
{
	quadVertices = new VertexTexture[6];

	quadVertices[0].Position = D3DXVECTOR3(-1.0f, -1.0f, 0);
	quadVertices[1].Position = D3DXVECTOR3(-1.0f,  1.0f, 0);
	quadVertices[2].Position = D3DXVECTOR3( 1.0f, -1.0f, 0);
	quadVertices[3].Position = D3DXVECTOR3( 1.0f, -1.0f, 0);
	quadVertices[4].Position = D3DXVECTOR3(-1.0f,  1.0f, 0);
	quadVertices[5].Position = D3DXVECTOR3( 1.0f,  1.0f, 0);

	quadVertices[0].Uv = D3DXVECTOR2(0, 1); // 0
	quadVertices[1].Uv = D3DXVECTOR2(0, 0); // 1
	quadVertices[2].Uv = D3DXVECTOR2(1, 1); // 2
	quadVertices[3].Uv = D3DXVECTOR2(1, 1); // 2
	quadVertices[4].Uv = D3DXVECTOR2(0, 0); // 1
	quadVertices[5].Uv = D3DXVECTOR2(1, 0); // 3

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * 6;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = quadVertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &quadBuffer);
		assert(SUCCEEDED(hr));
	}
}

float Sky::GetStarIntensity()
{
	return 0.0f;
}

D3DXVECTOR3 Sky::GetDirection()
{
	float y = (float)cosf(theta);
	float x = (float)(sinf(theta) * cosf(phi));
	float z = (float)(sinf(theta) * sinf(phi));

	return D3DXVECTOR3(x, y, z);
}
