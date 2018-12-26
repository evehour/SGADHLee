#include "Framework.h"
#include "Interfaces/IComponent.h"
#include "GameObject.h"

GameObject::GameObject()
{
	AddComponent<Transform>();
}

GameObject::~GameObject()
{
}

IComponent * GameObject::AddComponent(const ComponentType & type)
{
	return nullptr;
}

bool GameObject::HasComponent(const ComponentType & type)
{
	return false;
}

void GameObject::Initialize()
{
}

void GameObject::Update()
{
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
bool GameObject::HasCompoonent()
{
	return components.find(type) != components.end();
}

template<typename T>
void GameObject::RemoveComponent()
{
	ComponentType type = IComponent::DeduceComponentType<T>();

	if (components.find(type) == components.end())
		return;

	for (auto it = components.begin(); it != components.end();)
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
