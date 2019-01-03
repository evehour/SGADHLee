#include "Framework.h"
#include "Interfaces/IComponent.h"
#include "GameObject.h"

GameObject::GameObject()
{
	transform = AddComponent<Transform>();
}

GameObject::~GameObject()
{
}
IComponent * GameObject::GetComponent(const ComponentType & type)
{
	if (!HasComponent(type))
		return NULL;

	return components.find(type)->second;
}

IComponent * GameObject::AddComponent(const ComponentType & type)
{
	IComponent* tempComponent;
	
	if (HasComponent(type))
		return GetComponent(type);

	switch (type)
	{
		case ComponentType::Transform:
			tempComponent = new Transform(this);
			break;
	}

	tempComponent->SetComponentType(type);

	components.insert(make_pair(type, tempComponent));

	return tempComponent;
}

bool GameObject::HasComponent(const ComponentType & type)
{
	return (components.find(type) != components.end());
}

void GameObject::Initialize()
{
}

void GameObject::Update()
{
}

void GameObject::Active(bool val)
{
	bActive = val;

	for (auto temp : Actived)
		temp(val);
}

void GameObject::Enable(bool val)
{
	bEnable = val;

	for (auto temp : Enabled)
		temp(val);
}

void GameObject::Visible(bool val)
{
	bVisible = val;

	for (auto temp : Visibled)
		temp(val);
}

template<typename T>
T * GameObject::AddComponent()
{
	ComponentType type = IComponent::DeduceComponentType<T>();

	if (HasComponent(type))
		return GetComponent<T>();

	T* newComponent = new T(this);

	newComponent->SetComponentType(type);
	components.insert(make_pair(type, newComponent));

	return newComponent;
}

template<typename T>
T * GameObject::GetComponent()
{
	ComponentType type = IComponent::DeduceComponentType<T>();

	if (components.find(type) == components.end())
		return nullptr;

	return static_cast<T*>(components.find(type)->second);
}

template<typename T>
bool GameObject::HasComponent()
{
	return HasComponent(T->GetComponentType());
}

template<typename T>
void GameObject::RemoveComponent()
{
	ComponentType type = IComponent::DeduceComponentType<T>();

	if (components.find(type) == components.end())
		return;

	for (multimap<enum class ComponentType, class IComponent*>::iterator it = components.begin(); it != components.end();)
	{
		auto component = *it;
		if (type == component.second->GetType())
		{
			SAFE_DELETE(component.second);
			it = components.erase(it);
		}
		else
			it++;
	}
}
