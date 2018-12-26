#pragma once

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	const string& GetName() const { return name; }
	const bool& IsActive() const { return bActive; }

	void SetName(const string & name) { this->name = name; }
	void SetIsActive(const bool& bActive) { this->bActive = bActive; }

	class IComponent* AddComponent(const enum class ComponentType& type);
	template<typename T> T* AddComponent();
	template<typename T> T* GetComponent();

	bool HasComponent(const ComponentType& type);
	template<typename T> bool HasCompoonent();

	template<typename T> void RemoveComponent();

	void Initialize();
	void Update();

private:
	string name;
	bool bActive;

	multimap<enum class ComponentType, class IComponent*> components;
};
