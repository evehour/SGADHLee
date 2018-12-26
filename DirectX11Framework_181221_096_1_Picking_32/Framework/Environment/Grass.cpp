#include "Framework.h"
#include "Grass.h"
#include "./Environment/Terrain.h"
#include "./Viewer/Camera.h"

Grass::Grass()
	: grassCount(0), drawCount(0)
	, WindDirection(1,0), ShakeFactor(5.0f)
	, DeltaSTime(0)
	, textureArray(NULL)
{
}

Grass::~Grass()
{

}

void Grass::Initialize()
{
	intanceData = new InstData[20000];
}

void Grass::Ready(Terrain* terrain)
{
	this->terrain = terrain;

	shader = new Shader(Shaders + L"054_Grass.fx");

	Vertex vertex[1];// = new Vertex;
	vertex[0].Position = D3DXVECTOR3(0, 0, 0);

	//Create Vertex Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(Vertex);
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertex;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer[0]);
		assert(SUCCEEDED(hr));
	}

	//Create Instance Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(InstData) * 20000;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = intanceData;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer[1]);
		assert(SUCCEEDED(hr));
	}

	//textureStorage.push_back(Textures + L"Grass/Grass000.png");
	//textureStorage.push_back(Textures + L"Grass/Grass001.png");
	//textureArray = new TextureArray(textureStorage);

	//shader->AsShaderResource("Maps")->SetResource(textureArray->SRV());
}

void Grass::Update()
{
	DeltaSTime += ShakeFactor * Time::Delta();
	shader->AsScalar("DeltaSTime")->SetFloat(DeltaSTime);
}

void Grass::Render()
{
	//ImGui::Begin("Environment");
	//{
	//	ImGui::Separator();
	//	ImGui::Text("Grass");

	//	ImGui::SliderInt("Grass Count", (int*)&grassCount, 0, MaximumGrassCount);

	//	if(ImGui::SliderFloat2("Wind Direction", (float *)&WindDirection, -1.0f, 1.0f))
	//		shader->AsVector("WindDirection")->SetFloatVector(WindDirection);

	//	if(ImGui::SliderFloat("Wind Factor", &ShakeFactor, 0.0f, 5.0f))
	//		shader->AsScalar("ShakeFactor")->SetFloat(ShakeFactor);

	//	ImGui::Separator();
	//}
	//ImGui::End();

	UINT stride[2] = { sizeof(Vertex), sizeof(InstData) };
	UINT offset[2] = { 0, 0 };

	D3D::GetDC()->IASetVertexBuffers(0, 2, vertexBuffer, stride, offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	shader->DrawInstanced(0, 0, 1, grassCount);
}

void Grass::Destroy()
{
	{
		SAFE_RELEASE(pReadBackBuffer);
		SAFE_RELEASE(pBuffer2UAV);
		SAFE_RELEASE(pBuffer1UAV);
		SAFE_RELEASE(pBuffer2SRV);
		SAFE_RELEASE(pBuffer1SRV);
		SAFE_RELEASE(pBuffer2);
		SAFE_RELEASE(pBuffer1);
	}

	SAFE_DELETE(textureArray);
	for (UINT i = 0; i < 2; i++)
		SAFE_RELEASE(vertexBuffer[i]);

	SAFE_DELETE_ARRAY(intanceData);
	SAFE_DELETE(shader);
}

void Grass::AddGrassArea(const D3DXVECTOR3 & minPos, const D3DXVECTOR3 & maxPos, const UINT count, const InstData & initData)
{
	D3DXVECTOR3 _minPos, _maxPos;
	_minPos.x = min(minPos.x, maxPos.x);
	_minPos.z = min(minPos.z, maxPos.z);
	_maxPos.x = max(minPos.x, maxPos.x);
	_maxPos.z = max(minPos.z, maxPos.z);

	vector<InstData> tmpStorage;
	
	for (UINT i = 0; i < count; i++)
	{
		InstData data = initData;
		data.Position.x = Math::Random(_minPos.x, _maxPos.x);
		data.Position.z = Math::Random(_minPos.z, _maxPos.z);
		tmpStorage.push_back(data);
	}

	AddGrass(tmpStorage);
}

void Grass::AddGrass(vector<InstData>& package)
{
	for (vector<InstData>::iterator it = package.begin(); it != package.end(); ++it)
	{
		D3DXVECTOR3 pos = (*it).Position;
		pos.y = terrain->GetHeight(pos.x, pos.z) + ((*it).Scale.y * 0.5f);
		(*it).Position = pos;
	}

	vector<InstData> tmp;
	tmp.assign(grassStorage.size() + package.size(), InstData());

	if (grassStorage.size() > 0)
		memcpy(&tmp[0], &grassStorage[0], grassStorage.size() * sizeof(InstData));
	memcpy(&tmp[grassStorage.size()], &package[0], package.size() * sizeof(InstData));

	SetGrass(tmp);
}

void Grass::SetGrass(vector<InstData>& package)
{
	grassStorage.clear();
	grassStorage.assign(package.begin(), package.end());

	grassCount = min(grassStorage.size(), 20000);
#if false
	D3D::GetDC()->UpdateSubresource(vertexBuffer[1], 0, NULL, &glassStorage[0], grassCount * sizeof(InstData), 0);
#else
	D3D11_MAPPED_SUBRESOURCE subResource;
	HRESULT hr = D3D::GetDC()->Map
	(
		vertexBuffer[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
	);
	assert(SUCCEEDED(hr));

	memcpy(subResource.pData, &grassStorage[0], grassCount * sizeof(InstData));
	D3D::GetDC()->Unmap(vertexBuffer[1], 0);
#endif
}

void Grass::AddGrassTexture(const wstring & file)
{
	vector<wstring> tmp;
	tmp.assign(textureStorage.begin(), textureStorage.end());
	tmp.push_back(file);

	SetGrassTexture(tmp);
}

void Grass::SetGrassTexture(const vector<wstring> & files)
{
	textureStorage.clear();
	textureStorage.assign(files.begin(), files.end());

	SAFE_DELETE(textureArray);
	textureArray = new TextureArray(textureStorage);

	shader->AsShaderResource("Maps")->SetResource(textureArray->SRV());
}

void Grass::GetInstanceData(OUT vector<InstData>& datas)
{
	datas.clear();
	datas.assign(grassStorage.begin(), grassStorage.end());
}
