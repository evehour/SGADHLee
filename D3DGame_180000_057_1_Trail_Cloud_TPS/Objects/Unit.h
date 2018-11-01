#pragma once
#include "../Objects/DebugDraw.h"

class Unit
{
public:
	enum class Unit_State
	{
		Idle = 0, Moving, Attack, Hit, Dying, Max,
	};

	struct Unit_Status
	{
		float Hp;
		float Atk;

		Unit_Status()
		{
			Hp = 0.0f;
			Atk = 0.0f;
		}
	};

public:
	Unit(
		wstring matFolder, wstring matFile,
		wstring meshFolder, wstring meshFile,
		wstring shaderFolder = Shaders, wstring shaderFile = L"035_Animation_HW.hlsl"
	);

	Unit(
		wstring matFolder, wstring matFile,
		wstring meshFolder, wstring meshFile,
		Shader* shader
	);
	void UnitSetting();

	virtual ~Unit();

	virtual void Update();
	virtual void Render();

	bool IsEnable() { return bEnable; }
	virtual void InitUnit();
	virtual void EnableUnit();
	virtual void DisableUnit();

	class GameAnimModel* GetModel() { return model; }

public:
	void SetCurrentState(Unit::Unit_State val) { this->currentState = val; }
	Unit::Unit_State GetCurrentState() { return currentState; }

	void SetPrevState(Unit::Unit_State val) { this->prevState = val; }
	Unit::Unit_State GetPrevState() { return prevState; }
	void UpdateClip();

	class Collider* GetCollider() { return collider; }
	class Collider* GetAttackCollider() { return attackCollider; }

	void RestoreDiffuse();

	float GetDistanceOtherUnit(class Unit* toUnit);

protected:
	class Collider* collider;
	class ModelBone* colliderBone;

	class Collider* attackCollider;
	class ModelBone* attackBone;
	bool bAttackColliderEnable;

public:
	void SetColliderBone(class ModelBone* target) { this->colliderBone = target; }
	bool IsAttackColliderEnable() { return bAttackColliderEnable; }
	void SetAttackBone(class ModelBone* target) { this->attackBone = target; }

	Unit_Status unitStatus;

protected:
	bool bEnable;

	int state_max;
	Unit_State currentState;
	Unit_State prevState;

	class GameAnimModel* model;
	Shader* shader;

	class ModelClip** clips;

	bool isLeft;
	bool bSlash;
	bool bHit;

	bool bDebugDraw;
	class DebugDraw* debugDraw;

private:
	virtual void Hit();
};