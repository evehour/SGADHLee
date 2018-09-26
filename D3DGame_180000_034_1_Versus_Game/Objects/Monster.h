#pragma once
#include "../Objects/Unit.h"

class Monster : public Unit
{
public:
	Monster(
		wstring matFolder, wstring matFile,
		wstring meshFolder, wstring meshFile,
		wstring shaderFolder = Shaders, wstring shaderFile = L"035_Animation_HW.hlsl"
	);

	Monster(
		wstring matFolder, wstring matFile,
		wstring meshFolder, wstring meshFile,
		Shader* shader
	);
	~Monster();

	void Update();
	void Render();

	void SetPlayer(class Unit* unitPlayer) { this->unitPlayer = unitPlayer; }

private:
	void Idle();
	void Move();
	void Slash();
	void Dying();

private:
	float dyingTimer;
	class Unit* unitPlayer;

	float _searchDistance;
	float _attackDistance;
	float _moveSpeed;

private:
	void AnimationSetting();
	void ColliderSetting();
	void StatusSetting();
	void StateSetting();

	void SetLookAt(float deltaTime = 1.0f);

public:
	// Event
	void OnSlashEnd();
};