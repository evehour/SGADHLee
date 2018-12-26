#include "stdafx.h"
#include "TestTerrain.h"

#include "Environment/CubeSky.h"
#include "Environment/Terrain.h"
#include "Environment/TerrainBrush.h"
#include "Environment/PathFinding.h"
#include "Environment/HeightMap.h"

#include "Environment/Grass.h"

#include "Objects/GameObject.h"

void TestTerrain::Initialize()
{
	sky = new CubeSky(Textures + L"sunsetcube1024.dds");
	
	Terrain::InitDesc desc;
	// Create Terrain
	{
		desc =
		{
			Textures + L"Terrain/terrain.raw",
			50.0f,
			2049, 2049,
			{
				Textures + L"Terrain/grass.png",
				Textures + L"Terrain/darkdirt.png",
				Textures + L"Terrain/stone.png",
				Textures + L"Terrain/lightdirt.png",
				Textures + L"Terrain/snow.png",
			},
			Textures + L"Terrain/blend.png",

			0.5f,
		};

		terrain = new Terrain(desc);
#ifdef __USING_PATHFINDING__
		pathFinding = new PathFinding(terrain);
#endif
		terrain->Initialize();
	}

	grass = new Grass();
	grass->Initialize();


	picked = false;

	bBrushHovering = false;

	brush = new TerrainBrush(terrain, 2049, 2049);
	brush->Initialize();
	selectedBrush = 0U;
	editorMode = EditorMode::TerrainBrush;
}

void TestTerrain::Ready()
{
	emptyTexture = new Texture(Textures + L"Empty.png");

	typeIcon[0] = new Texture(Textures + L"Icon/Brush.png");
	typeIcon[1] = new Texture(Textures + L"Icon/Object.png");
	typeIcon[2] = new Texture(Textures + L"Icon/Plant.png");
	typeIcon[3] = new Texture(Textures + L"Icon/Config.png");

	// Terrain Setting
	{
#ifdef __USE_BLENDMAP__
		terrainMaterial = new Material(Shaders + L"060_Terrain.fx");
#else
		terrainMaterial = new Material(Shaders + L"060_Terrain_InBlending.fx");
#endif
		terrain->Ready(terrainMaterial);
	}

	// Grass Setting
	{
		grass->Ready(terrain);
		plantType = -1;

		plantSensitive = 1.0f;
		plantWindLimitFactor = 0.4f;
		plantScale = D3DXVECTOR2(1, 1);
		plantShakeType = 0;
		plantInsertCount = 1;
	}

	// Brush Setting
	{
		brush->Ready();

		{
			Texture* t;
			int i = 0;

			// ImGui가 이미지 텍스쳐 SRV의 ID를 갖고 구분하므로
			// 각각의 이미지 파일은 모두 달라야 한다.

			t = new Texture(Textures + L"Moon.png");
			brushes.insert(make_pair(i++, BrushPair(L"Circle", t)));

			t = new Texture(Textures + L"Red.png");
			brushes.insert(make_pair(i++, BrushPair(L"Square", t)));

			t = new Texture(Textures + L"Tree.png");
			brushes.insert(make_pair(i++, BrushPair(L"Tree", t)));
		}

		currentBrushShape = 0;
		brush->SetBrush(brushes[currentBrushShape].second);
		brush->Power(0.05f);
	}


}

void TestTerrain::Destroy()
{
	SAFE_DELETE(emptyTexture);

	for (UINT i = 0; i < 4; i++)
		SAFE_DELETE(typeIcon[i]);

	for (map<int, BrushPair>::iterator it = brushes.begin(); it != brushes.end();)
	{
		SAFE_DELETE(it->second.second);
		it = brushes.erase(it);
	}

	SAFE_DELETE(brush);
#ifdef __USING_PATHFINDING__
	SAFE_DELETE(pathFinding);
#endif

	for (UINT i = 0; i < plantTextures.size(); i++)
		SAFE_DELETE(plantTextures[i]);
	grass->Destroy();
	SAFE_DELETE(grass);

	SAFE_DELETE(terrain);
	SAFE_DELETE(terrainMaterial);
	SAFE_DELETE(sky);
}

void TestTerrain::Update()
{
	terrain->Update();
	grass->Update();

	D3DXVECTOR3 mousePosition = Mouse::Get()->GetPosition();
	if (mousePosition != MousePosition)
	{
		MousePosition = mousePosition;
		picked = terrain->Picking(&pickPixel, &pickWorldPosition);

#ifdef __USING_PATHFINDING__
		MapTile* mapTile = NULL;
		mapTile = pathFinding->GetTile(D3DXVECTOR2(pickWorldPosition.x, pickWorldPosition.z));
		if (mapTile != NULL)
			pickMapTileWorldPos = mapTile->WorldPosition;
#endif

		brush->Hovering(picked);
		brush->UpdatePosition(pickPixel);
	}

	if (picked && !bBrushHovering)
	{
		switch (editorMode)
		{
			case EditorMode::TerrainBrush:
				if (Mouse::Get()->Press(0))
				{
					brush->Brush((TerrainBrush::BrushType)selectedBrush, !Keyboard::Get()->Press(VK_LSHIFT));
				}
				break;

			case EditorMode::PlantSpot:
				if (Mouse::Get()->Down(0))
				{
					Grass::InstData grassData;
					grassData.Scale = plantScale;
					grassData.ShakeType = plantShakeType;
					grassData.Type = (plantType < 0 ? 0 : plantType);
					grassData.WindLimitFactor = plantWindLimitFactor;

					D3DXVECTOR3 minPos, maxPos;
					minPos = maxPos = pickWorldPosition;
					float dx = 0.25f * brush->Scale().x * plantSensitive;
					float dy = 0.25f * brush->Scale().y * plantSensitive;

					minPos.x -= dx;
					minPos.z -= dy;
					maxPos.x += dx;
					maxPos.z += dy;

					grass->AddGrassArea(minPos, maxPos, (UINT)plantInsertCount, grassData);
				}
				break;
		}

		if (Keyboard::Get()->Press(VK_LCONTROL))
		{
			D3DXVECTOR2 dp(0.5f, 0.5f);
			D3DXVECTOR3 wheelMouseValue;
			wheelMouseValue = Mouse::Get()->GetMoveValue();

			if (wheelMouseValue.z != 0.0f)
			{
				D3DXVECTOR2 scale = brush->Scale();
				scale += dp * (wheelMouseValue.z > 0.0f ? 1.0f : -1.0f);
				
				if (scale.x < 0.0f)
					scale = D3DXVECTOR2(0, 0);

				brush->Scale(scale);
			}
		}//Keyboard::Get()->Press(VK_LCONTROL)
	}//picked && !bMouseInImgui
}

void TestTerrain::PreRender()
{
}

void TestTerrain::Render()
{
	sky->Render();
	terrain->Render();
	grass->Render();
	//-----------------------------------------------------------------------------
	// Test
	//brush->TestRender();
	//-----------------------------------------------------------------------------

	//-----------------------------------------------------------------------------
	ImGui::Begin("Debug");
	{
		ImGui::Separator();
		ImGui::Text("Pick: %s", (picked ? "True" : "False"));
		Terrain::InitDesc desc = terrain->Desc();
		ImGui::Text("Brush: %f, %f", pickPixel.x, pickPixel.z);
		ImGui::Text("PickWPos: %f, %f", pickWorldPosition.x, pickWorldPosition.z);
		if (picked)
		{
#ifdef __USING_PATHFINDING__
			ImGui::Text("PickMTWPos: %f, %f", pickMapTileWorldPos.x, pickMapTileWorldPos.z);
#endif
		}
		ImGui::Text("Mouse: %f, %f", Mouse::Get()->GetPosition().x, Mouse::Get()->GetPosition().y);
		ImGui::Separator();
	}
	ImGui::End();
	//-----------------------------------------------------------------------------

	ImGui::Begin("Brush");
	{
		bBrushHovering = false;
		bBrushHovering = ImGui::IsMouseHoveringWindow();

		{
			ImGui::Separator();

			ImGui::InvisibleButton("Void1", ImVec2(50, 1));
			ImGui::SameLine();
			if (ImGui::ImageButton(typeIcon[0]->SRV(), ImVec2(30, 20), ImVec2(0, 0.01f), ImVec2(1, 1)))
			{
				// Restore brush shape.
				brush->SetBrush(brushes[currentBrushShape].second);
				
				editorMode = EditorMode::TerrainBrush;
			}
			ImGui::SameLine();
			ImGui::InvisibleButton("Void1", ImVec2(10, 1));
			ImGui::SameLine();
			if (ImGui::ImageButton(typeIcon[1]->SRV(), ImVec2(30, 20), ImVec2(0, 0), ImVec2(1, 1)))
			{
				// Fixed brush shape.
				brush->SetBrush(brushes[1].second);

				editorMode = EditorMode::ObjectSpot;
			}
			ImGui::SameLine();
			ImGui::InvisibleButton("Void1", ImVec2(10, 1));
			ImGui::SameLine();
			if (ImGui::ImageButton(typeIcon[2]->SRV(), ImVec2(30, 20), ImVec2(0, 0), ImVec2(1, 1)))
			{
				// Fixed brush shape.
				brush->SetBrush(brushes[1].second);

				editorMode = EditorMode::PlantSpot;
			}
			ImGui::SameLine();
			ImGui::InvisibleButton("Void1", ImVec2(10, 1));
			ImGui::SameLine();
			if (ImGui::ImageButton(typeIcon[3]->SRV(), ImVec2(30, 20), ImVec2(0, 0.1f), ImVec2(1, 1)))
				editorMode = EditorMode::Config;
		}
		ImGui::Separator();

		switch (editorMode)
		{
			case EditorMode::TerrainBrush:
				UITerrainBrush();
				break;
			case EditorMode::ObjectSpot:
				UIObjectBrush();
				break;
			case EditorMode::PlantSpot:
				UIPlantBrush();
				break;
			case EditorMode::Config:
				break;
		}

	}
	ImGui::End();
}


void TestTerrain::PostRender()
{
}

void TestTerrain::ResizeScreen()
{
}

void TestTerrain::UITerrainBrush()
{
	// Brush Mode
	{
		ImGui::Text("Brush Mode");
		if (ImGui::Button("Normal", ImVec2(100, 25)))
			selectedBrush = (UINT)TerrainBrush::BrushType::Normal;
		ImGui::SameLine();
		if (ImGui::Button("Flat", ImVec2(100, 25)))
			selectedBrush = (UINT)TerrainBrush::BrushType::Flat;
		ImGui::SameLine();
		if (ImGui::Button("Smooth", ImVec2(100, 25)))
			selectedBrush = (UINT)TerrainBrush::BrushType::Smooth;

		float brushPower = brush->Power();
		ImGui::SliderFloat("Brush Power", &brushPower, 0.05f, 0.5f, "%.2f");
		brush->Power(brushPower);
	}

	ImGui::Separator();

	// Brush Shape
	{
		ImGui::Text("Brush Shape");
		for (UINT i = 0; i < brushes.size(); i++)
		{
			Texture* t = brushes[i].second;
			ImVec2 size(25.0f, 25.0f);

			if (i == currentBrushShape)
				size.x = size.y = 40.0f;

			if (ImGui::ImageButton(t->SRV(), size, ImVec2(0, 0), ImVec2(1, 1)))
			{
				currentBrushShape = i;
				brush->SetBrush(t);
			}

			if (((i + 1) % 6) != 0)
			{
				if (i < (brushes.size() - 1))
					ImGui::SameLine();
			}
		}
	}
	ImGui::Separator();
}

void TestTerrain::UIObjectBrush()
{
}

void TestTerrain::UIPlantBrush()
{
	if (ImGui::Button("SaveTestButton"))
	{
#if true
		brush->SaveHeightMap(L"terrain.raw");
#else
		ID3D11Texture2D* heightMapTex = terrain->GetHeightMapTexture();
		ID3D11ShaderResourceView* heightMapSRV = terrain->GetHeightMapSRV();
		ID3D11Resource* resource;
		heightMapSRV->GetResource(&resource);
		terrain->GetHeightMap()->SaveFile2(L"terrain.raw", &heightMapSRV);
		//terrain->GetHeightMap()->SaveFile(L"BBBB.raw", &resource);
#endif
	}



	ImGui::Text("Plant Option");
	ImGui::SliderFloat("Factor##Plant", &plantWindLimitFactor, 0.0f, 0.75f);
	ImGui::DragFloat2("Scale##Plant", (float*)&plantScale, 0.01f, 0.01f, 50.0f);
	ImGui::SliderInt("Shake Type##Plant", &plantShakeType, 0, 1);

	ImGui::Separator();
	ImGui::Text("Insert Option");
	ImGui::SliderFloat("Sensitive##Plant", &plantSensitive, 0.0f, 1.0f);
	ImGui::SliderInt("Count##Plant", &plantInsertCount, 1, 100);

	if (ImGui::Button("Add Texture"))
	{
		ImGui::OpenPopup("AddGrassTexture");
	}

	if (ImGui::BeginPopupModal("AddGrassTexture", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		bBrushHovering = true;

		static int plantSelectItemList = 0;
		vector<wstring> files;
		vector<wstring> filesOnlyName;
		vector<string> filesS;
		char** filesC;
		UINT _size;

		Path::GetFiles(&files, Textures, L"*.png", true);
		for (UINT i = 0; i < files.size(); i++)
		{
			filesOnlyName.push_back(Path::GetFileName(files[i]));
			filesS.push_back(String::ToString(filesOnlyName[i]));
		}

		_size = files.size();
		filesC = new char*[_size];
		for (UINT i = 0; i < _size; i++)
		{
			filesC[i] = new char[filesS[i].length() + 1];
			strcpy_s(filesC[i], filesS[i].length() + 1, filesS[i].c_str());
		}

		ImGui::ListBox("", &plantSelectItemList, filesC, _size);

		if (ImGui::Button("OK##PlantAT"))
		{
			plantTextures.push_back(new Texture(files[plantSelectItemList]));
			plantTextureFilenames.push_back(filesC[plantSelectItemList]);
			plantTextureFiles.push_back(files[plantSelectItemList]);
			grass->AddGrassTexture(files[plantSelectItemList]);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::Button("Cancel##PlantAT"))
		{
			ImGui::CloseCurrentPopup();
		}

		for (UINT i = 0; i < _size; i++)
			SAFE_DELETE(filesC[i]);
		SAFE_DELETE_ARRAY(filesC);

		ImGui::EndPopup();
	}

	int plantTexturesSize = (int)plantTextures.size();
	for (int i = 0; i < plantTexturesSize; i++)
	{
		ImGui::Bullet(); ImGui::SameLine();
		ImVec2 size(50, 50);
		if (plantType == i)
			size.x = size.y = 95;

		if (ImGui::ImageButton(plantTextures[i]->SRV(), size))
			plantType = i;
		ImGui::SameLine();
		ImGui::Text("%s", plantTextureFilenames[i].c_str());
	}

	if (plantTexturesSize < 1)
	{
		ImGui::Bullet(); ImGui::SameLine();
		ImGui::Image(emptyTexture->SRV(), ImVec2(50, 50));
	}
}
