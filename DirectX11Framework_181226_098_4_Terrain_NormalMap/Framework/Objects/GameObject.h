#pragma once

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	virtual void Initialize();
	virtual void Update();

	const string& GetName() const { return name; }
	void SetName(const string & name) { this->name = name; }

	void Active(bool val);
	bool Active() { return bActive; }
	vector<function<void(bool)>> Actived;

	void Enable(bool val);
	bool Enable() { return bEnable; }
	vector<function<void(bool)>> Enabled;

	void Visible(bool val);
	bool Visible() { return bVisible; }
	vector<function<void(bool)>> Visibled;

	class IComponent* AddComponent(const enum class ComponentType& type);
	template<typename T> T* AddComponent();

	class IComponent* GetComponent(const ComponentType & type);
	template<typename T> T* GetComponent();

	bool HasComponent(const ComponentType& type);
	template<typename T> bool HasComponent();

	template<typename T> void RemoveComponent();

protected:
	string name;
	bool bActive;
	bool bEnable;
	bool bVisible;

	multimap<enum class ComponentType, class IComponent*> components;

	Transform* transform;
};
