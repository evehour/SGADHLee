#pragma once
#include "../Objects/Unit.h"

class Player : public Unit
{
public:
	Player(
		wstring matFolder, wstring matFile,
		wstring meshFolder, wstring meshFile,
		wstring shaderFolder = Shaders, wstring shaderFile = L"035_Animation_HW.hlsl"
	);

	Player(
		wstring matFolder, wstring matFile,
		wstring meshFolder, wstring meshFile,
		Shader* shader
	);
	~Player();

	void Update();
	void Render();

	void SetTerrain(class Terrain* terrain) { this->terrain = terrain; }

	void Hit();
	class TrailRender* trailRender;

private:
	class Terrain* terrain;
	int idx;

private:
	void AnimationSetting();
	void ColliderSetting();
	void StatusSetting();

public:
	// Event
	void OnSlashEnd();
	void OnHitEnd();
	void OnEnableCollider();
	void OnDisableCollider();

private:
	void Move();
	void Slash();
};