#include "stdafx.h"
#include "Terrain.h"

Terrain::Terrain(ExecuteValues * values, Material * material)
	: values(values), material(material)
	, brushMode(0), texNum(1), strength(1.0)
{
	heightTexture = new Texture(Contents + L"HeightMaps/HeightMap256.png");
	colorTexture = new Texture(Textures + L"Dirt.png");
	colorTexture2 = new Texture(Textures + L"Dirt2.png");
	colorTexture3 = new Texture(Textures + L"Wall.png");

	D3DX11_IMAGE_LOAD_INFO info;
	ZeroMemory(&info, sizeof(D3DX11_IMAGE_LOAD_INFO));
	info.Width = 256;
	info.Height = 256;
	info.MipLevels = 1;
	info.Usage = D3D11_USAGE_STAGING;
	info.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	info.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE;
	info.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	info.Filter = D3DX11_FILTER_NONE;
	info.MipFilter = D3DX11_FILTER_NONE;

	alphaTexture = new Texture(Contents + L"HeightMaps/InitColorMap256.png", &info);

	ID3D11Texture2D* _alphaTexture;
	alphaTexture->GetView()->GetResource((ID3D11Resource **)&_alphaTexture);
	D3D11_TEXTURE2D_DESC _alphaDesc;
	_alphaTexture->GetDesc(&_alphaDesc);

	alphaFormat = _alphaDesc.Format;
	SAFE_RELEASE(_alphaTexture);

	worldBuffer = new WorldBuffer();
	brushBuffer = new BrushBuffer();

	CreateData();
	CreateNormalData();
	CreateBuffer();

	//Create Rasterizer
	{
		rasterizer[0] = new RasterizerState();
		rasterizer[1] = new RasterizerState();
		rasterizer[0]->FillMode(D3D11_FILL_WIREFRAME);
	}
}

Terrain::~Terrain()
{
	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	SAFE_DELETE(material);

	SAFE_DELETE(heightTexture);
	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(brushBuffer);

	SAFE_DELETE(rasterizer[0]);
	SAFE_DELETE(rasterizer[1]);
}

void Terrain::Update()
{
	if (D3DXVec3Length(&brushBuffer->Data.Location) > 0.0f)
	{
		UINT type = brushBuffer->Data.Type;
		//if (type == 1 && Mouse::Get()->Press(0))
		if (Mouse::Get()->Press(0))
		{
			switch (brushMode)
			{
			case 0:
				AdjustY(brushBuffer->Data.Location);
				break;
			case 1:
				AdjustAlpha(brushBuffer->Data.Location);
				break;
			}
		}
	}
}

void Terrain::Render()
{
	if (ImGui::Button("Wide View"))
	{
		values->MainCamera->Position(114.16f, 277.95f, -6.76f);
		values->MainCamera->RotationDegree(66.88f, -0.01f);
	}
	if (ImGui::Button("Close view"))
	{
		values->MainCamera->Position(121.93f, 86.71f, 26.64f);
		values->MainCamera->RotationDegree(84.50f, 0.0f);
	}
	ImGui::Separator();
	ImGui::Text("Brush");
	ImGui::Separator();

	ImGui::Checkbox("Terrain Wireframe", &bWireFrame);
	ImGui::Separator();

	ImGui::SliderInt("Mode", &brushMode, 0, 1);
	ImGui::Separator();
	ImGui::SliderInt("Type", &brushBuffer->Data.Type, 1, 3);
	ImGui::SliderInt("Range", &brushBuffer->Data.Range, 1, 5);
	ImGui::SliderFloat3("Color", (float *)&brushBuffer->Data.Color, 0, 1);
	if (brushMode == 1)
	{
		ImGui::Separator();
		ImGui::SliderInt("Tex", &texNum, 1, 2);
		ImGui::SliderFloat("Strength", &strength, 1.0f, 10.0f);
	}
	ImGui::Separator();
	ImGui::Spacing();
	if (ImGui::Button("Save alpha")) SaveAlphaMap(L"../_Contents/HeightMaps/InitColorMap256.png");
	brushBuffer->SetVSBuffer(10);


	alphaTexture->SetShaderResource(10);
	alphaTexture->SetShaderSampler(10);

	colorTexture->SetShaderResource(11);
	colorTexture->SetShaderSampler(11);

	colorTexture2->SetShaderResource(12);
	colorTexture2->SetShaderSampler(12);

	colorTexture3->SetShaderResource(13);
	colorTexture3->SetShaderSampler(13);

	UINT stride = sizeof(VertexColorTextureNormal);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	worldBuffer->SetVSBuffer(1);
	material->PSSetBuffer();

	if (bWireFrame)
		rasterizer[1]->RSSetState();

	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	rasterizer[0]->RSSetState();
}

bool Terrain::Y(OUT D3DXVECTOR3 * out, D3DXVECTOR3 & position)
{
	D3DXVECTOR3 start = D3DXVECTOR3(position.x, 1000, position.z);
	D3DXVECTOR3 direction = D3DXVECTOR3(0, -1, 0);

	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x >= width) return false;
	if (z < 0 || z >= height) return false;

	UINT index[4];
	index[0] = (width + 1) * z + x;
	index[1] = (width + 1) * (z + 1) + x;
	index[2] = (width + 1) * z + (x + 1);
	index[3] = (width + 1) * (z + 1) + x + 1;

	D3DXVECTOR3 p[4];
	for (int i = 0; i < 4; i++)
		p[i] = vertices[index[i]].Position;

	float u, v, distance;
	if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance))
	{
		*out = p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;

		return true;
	}

	if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance))
	{
		*out = p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;

		return true;
	}

	return false;
}

bool Terrain::Y(OUT D3DXVECTOR3 * out)
{
	D3DXVECTOR3 start;
	values->MainCamera->Position(&start);

	D3DXVECTOR3 direction = values->MainCamera->Direction
	(
		values->Viewport, values->Perspective
	);
	
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			UINT index[4];
			index[0] = (width + 1) * z + x;
			index[1] = (width + 1) * (z + 1) + x;
			index[2] = (width + 1) * z + (x + 1);
			index[3] = (width + 1) * (z + 1) + x + 1;

			D3DXVECTOR3 p[4];
			for (int i = 0; i < 4; i++)
				p[i] = vertices[index[i]].Position;

			float u, v, distance;
			if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance))
			{
				*out = p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;
				brushBuffer->Data.Location = *out;

				return true;
			}

			if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance))
			{
				*out = p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;
				brushBuffer->Data.Location = *out;

				return true;
			}
		}//for(x)
	}//for(z);
	
	return false;
}

void Terrain::AdjustY(D3DXVECTOR3 & location)
{
	UINT size = (UINT)brushBuffer->Data.Range;
	float dx, dz, dist, ddist;

	D3D11_BOX box;
	box.left = (UINT)location.x - size;
	box.top = (UINT)location.z + size;
	box.right = (UINT)location.x + size;
	box.bottom = (UINT)location.z - size;

	if (box.left < 0)box.left = 0;
	if (box.top >= height)box.top = height;
	if (box.right >= width)box.right = width;
	if (box.bottom < 0)box.bottom = 0;

	for (UINT z = box.bottom; z <= box.top; z++)
	{
		for (UINT x = box.left; x <= box.right; x++)
		{
			UINT index = (width + 1) * z + x;
			int mode = brushBuffer->Data.Type;

			switch (mode)
			{
			case 1:
				vertices[index].Position.y += 10.0f * Time::Delta();
				break;
			case 2:
				dx = vertices[index].Position.x - location.x;
				dz = vertices[index].Position.z - location.z;
				dist = sqrt(dx * dx + dz * dz);
				if (dist > size) continue;
				vertices[index].Position.y += 10.0f * Time::Delta();
				break;
			case 3:
				dx = vertices[index].Position.x - location.x;
				dz = vertices[index].Position.z - location.z;
				dist = sqrt(dx * dx + dz * dz);
				if (dist > size) continue;
				dist = (size - dist) / (float)size * (D3DX_PI / 2.0f);
				vertices[index].Position.y += ((sinf(dist) * (float)size)/* + 10.0f*/) * Time::Delta();
				break;
			default:
				break;
			}
		}
	}
	CreateNormalData();

	D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, &vertices[0], sizeof(VertexColorTextureNormal), vertexCount);

}

void Terrain::AdjustAlpha(D3DXVECTOR3 & location)
{
	UINT size = (UINT)brushBuffer->Data.Range;
	float dx, dz, dist, ddist;
	int _texNum = texNum - 1;

	D3D11_BOX box;
	box.left = (UINT)location.x - size;
	box.top = (UINT)location.z + size;
	box.right = (UINT)location.x + size;
	box.bottom = (UINT)location.z - size;

	if (box.left < 0)box.left = 0;
	if (box.top >= height)box.top = height;
	if (box.right >= width)box.right = width;
	if (box.bottom < 0)box.bottom = 0;

	for (UINT z = box.bottom; z <= box.top; z++)
	{
		for (UINT x = box.left; x <= box.right; x++)
		{
			UINT index = (width + 1) * z + x;
			int mode = brushBuffer->Data.Type;

			switch (mode)
			{
			case 1:
				alphaColorBuffer[index][_texNum] += 100.0f / 255.0f * (Time::Delta() * strength);
				//alphaColorBuffer[index][_texNum] += 100.0f * (Time::Delta() * strength);
				break;
			case 2:
				dx = vertices[index].Position.x - location.x;
				dz = vertices[index].Position.z - location.z;
				dist = sqrt(dx * dx + dz * dz);
				if (dist > size) continue;
				alphaColorBuffer[index][_texNum] += 100.0f / 255.0f * (Time::Delta() * strength);
				//alphaColorBuffer[index][_texNum] += 100.0f * (Time::Delta() * strength);
				break;
			case 3:
				dx = vertices[index].Position.x - location.x;
				dz = vertices[index].Position.z - location.z;
				dist = sqrt(dx * dx + dz * dz);
				if (dist > size) continue;
				dist = ((size - dist) * 255.0f) / (float)(size * 255.0f) * (D3DX_PI / 2.0f);
				alphaColorBuffer[index][_texNum] += 100.0f / 255.0f * dist * (Time::Delta() * strength);
				//alphaColorBuffer[index][_texNum] += 100.0f * dist * (Time::Delta() * strength);
				break;
			default:
				break;
			}

			if (alphaColorBuffer[index][_texNum] > 1.0f) alphaColorBuffer[index][_texNum] = 1.0f;
			//if (alphaColorBuffer[index][_texNum] > 255.0f) alphaColorBuffer[index][_texNum] = 255.0f;
			vertices[index].Color[_texNum] = alphaColorBuffer[index][_texNum];
			/*if (vertices[index].Color[_texNum] > 1.0f) vertices[index].Color[_texNum] = 1.0f;
			vertices[index].Color[_texNum] += (100.0f / 255.0f) * (Time::Delta() * strength);*/
		}
	}
	//CreateNormalData();
	//alphaTexture->WritePixels(DXGI_FORMAT_R8G8B8A8_UNORM, alphaColorBuffer, true);
	D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, &vertices[0], sizeof(VertexColorTextureNormal), vertexCount);
}

float Terrain::Y(D3DXVECTOR3 & position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x >= width) return 0.0f;
	if (z < 0 || z >= height) return 0.0f;


	UINT index[4];
	index[0] = (width + 1) * z + x;
	index[1] = (width + 1) * (z + 1) + x;
	index[2] = (width + 1) * z + (x + 1);
	index[3] = (width + 1) * (z + 1) + x + 1;

	D3DXVECTOR3 v[4];
	for (int i = 0; i < 4; i++)
		v[i] = vertices[index[i]].Position;


	float ddx = (position.x - v[0].x) / 1.0f;
	float ddz = (position.z - v[0].z) / 1.0f;

	D3DXVECTOR3 temp;

	if (ddx + ddz <= 1)
	{
		temp = v[0] + (v[2] - v[0]) * ddx + (v[1] - v[0]) * ddz;
	}
	else
	{
		ddx = 1 - ddx;
		ddz = 1 - ddz;

		temp = v[3] + (v[1] - v[3]) * ddx + (v[2] - v[3]) * ddz;
	}

	return temp.y;
}

void Terrain::CreateData()
{
	vector<D3DXCOLOR> heights;
	heightTexture->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &heights);

	width = heightTexture->GetWidth() - 1;
	height = heightTexture->GetHeight() - 1;

	//Create VertexData
	{
		vertexCount = (width + 1) * (height + 1);
		vertices = new VertexColorTextureNormal[vertexCount];
		
		for (UINT z = 0; z <= height; z++)
		{
			for (UINT x = 0; x <= width; x++)
			{
				UINT index = (width + 1) * z + x;

				vertices[index].Position.x = (float)x;
				vertices[index].Position.y = (float)(heights[index].r * 255.0f) / 10.0f;
				//vertices[index].Position.y = 0.0f;
				vertices[index].Position.z = (float)z;

				vertices[index].Uv.x = (float)x / width;
				vertices[index].Uv.y = (float)z / height;
			}
		}//for(z)
	}


	//CreateIndexData
	{
		indexCount = width * height * 6;
		indices = new UINT[indexCount];

		UINT index = 0;
		for (UINT z = 0; z < height; z++)
		{
			for (UINT x = 0; x < width; x++)
			{
				indices[index + 0] = (width + 1) * z + x; //0
				indices[index + 1] = (width + 1) * (z + 1) + x; //1
				indices[index + 2] = (width + 1) * z + x + 1; //2

				indices[index + 3] = (width + 1) * z + x + 1; //2
				indices[index + 4] = (width + 1) * (z + 1) + x; //1
				indices[index + 5] = (width + 1) * (z + 1) + x + 1; //1

				index += 6;
			}
		}//for(z)
	}

	//CreateAlphaColorMap
	{
		CreateColorData(width + 1, height + 1);
	}
}

void Terrain::CreateNormalData()
{
	for (UINT i = 0; i < indexCount / 3; i++)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		VertexColorTextureNormal v0 = vertices[index0];
		VertexColorTextureNormal v1 = vertices[index1];
		VertexColorTextureNormal v2 = vertices[index2];

		D3DXVECTOR3 d1 = v1.Position - v0.Position;
		D3DXVECTOR3 d2 = v2.Position - v0.Position;

		D3DXVECTOR3 normal;
		D3DXVec3Cross(&normal, &d1, &d2);

		vertices[index0].Normal += normal;
		vertices[index1].Normal += normal;
		vertices[index2].Normal += normal;
	}

	for (UINT i = 0; i < vertexCount; i++)
		D3DXVec3Normalize(&vertices[i].Normal, &vertices[i].Normal);
}

void Terrain::CreateBuffer()
{
	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexColorTextureNormal) * vertexCount;
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
}

void Terrain::CreateColorData(UINT width, UINT height)
{
#if false
	alphaTexture->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &alphaColorBuffer, true);
#else
	alphaTexture->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &alphaColorBuffer);
	for (int y = 0; y < height ; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int vIndex = height * y + x;
			vertices[vIndex].Color = alphaColorBuffer[vIndex];
		}
	}
#endif
}

void Terrain::SaveAlphaMap(wstring fileName)
{
#if true
	alphaTexture->WritePixels(DXGI_FORMAT_R8G8B8A8_UNORM, alphaColorBuffer);
	alphaTexture->SaveFile(fileName);
#else
	ID3D11Texture2D *pTex = NULL;
	HRESULT hr;

	ID3D11Texture2D* _alphaTexture;
	alphaTexture->GetView()->GetResource((ID3D11Resource **)&_alphaTexture);
	D3D11_TEXTURE2D_DESC srcDesc;
	_alphaTexture->GetDesc(&srcDesc);

	D3D11_TEXTURE2D_DESC destDesc;
	ZeroMemory(&destDesc, sizeof(D3D11_TEXTURE2D_DESC));
	destDesc.Width = srcDesc.Width;
	destDesc.Height = srcDesc.Height;
	destDesc.MipLevels = destDesc.ArraySize = 1;
	destDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	destDesc.SampleDesc = srcDesc.SampleDesc;
	destDesc.Usage = D3D11_USAGE_STAGING;
	destDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	destDesc.MiscFlags = 0;

	hr = D3D::GetDevice()->CreateTexture2D(&destDesc, NULL, &pTex);
	assert(SUCCEEDED(hr));

	D3D11_MAPPED_SUBRESOURCE  mapResource;
	D3D::GetDC()->Map(pTex, 0, D3D11_MAP_WRITE, NULL, &mapResource);
	{
		for (int y = 0; y < destDesc.Height; y++)
		{
			for (int x = 0; x < destDesc.Width; x++)
			{
				UINT index = destDesc.Width * y + x;
				D3DXCOLOR color = vertices[index].Color * 255.0f;

				*((UINT*)(mapResource.pData) + index) =
					((UINT)(color.a) << 24)
					+ ((UINT)(color.b) << 16)
					+ ((UINT)(color.g) << 8)
					+ ((UINT)(color.r) << 0);
			}
		}
	}
	D3D::GetDC()->Unmap(pTex, 0);

	hr = D3DX11SaveTextureToFile(D3D::GetDC(), pTex, D3DX11_IFF_PNG, fileName.c_str());
	assert(SUCCEEDED(hr));
	SAFE_RELEASE(pTex);
#endif
}


