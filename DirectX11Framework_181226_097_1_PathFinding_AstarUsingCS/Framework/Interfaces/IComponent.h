#pragma once
#include "Framework.h"

enum class ComponentType : UINT
{
	Unknown,
	Transform,
};

class IComponent
{
public:
	template<typename T>
	static const ComponentType DeduceComponentType();

public:
	IComponent(class GameObject* owner)
		: owner(owner)
		, componentType(ComponentType::Unknown)
		, bEnabled(false) {};
	virtual ~IComponent() {}

	virtual void Initialize() = 0;
	virtual void Ready() = 0;
	virtual void Update() = 0;
	virtual void PreRender() = 0;
	virtual void Render() = 0;
	virtual void PostRender() = 0;

	class GameObject* GetOwner() const { return owner; }
	const ComponentType& GetComponentType() const { return componentType; }
	const UINT& GetComponentID() const { return componentID; }
	const bool& IsEnable() const { return bEnabled; }

	void SetComponentType(const ComponentType& componentType) { this->componentType = componentType; }
	void SetComponentID(const UINT& componentID) { this->componentID = componentID; }
	void SetEnable(const bool& bEnable) { this->bEnabled = bEnable; }

protected:
	class GameObject* owner;
	ComponentType componentType;
	UINT componentID;
	bool bEnabled;
};

template<typename T>
inline const ComponentType IComponent::DeduceComponentType()
{
	string _componentType = typeid(T).name();
	ComponentType enumType = ComponentType::Unknown;

	if (_componentType.find("Transform") != string::npos) enumType = ComponentType::Transform;

	return enumType;
}
