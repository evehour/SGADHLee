#include "stdafx.h"
#include "Base3DParticleInstance.h"
#include "Base3DParticleInstancer.h"

Base3DParticleInstance::Base3DParticleInstance()
{
	ID++;
	id = ID;
}

Base3DParticleInstance::Base3DParticleInstance(D3DXVECTOR3 position, D3DXVECTOR3 scale, Base3DParticleInstancer * instancer, D3DXQUATERNION orientation)
	: Base3DParticleInstance(position, scale, instancer)
{
	this->orientation = orientation;
	this->Update();
}

Base3DParticleInstance::Base3DParticleInstance(D3DXVECTOR3 position, D3DXVECTOR3 scale, D3DXVECTOR3 mods, Base3DParticleInstancer * instancer)
	: Base3DParticleInstance(position, scale, instancer)
{
	this->pMods = mods;
	this->Update();
}

Base3DParticleInstance::Base3DParticleInstance(D3DXVECTOR3 position, D3DXVECTOR3 scale, Base3DParticleInstancer * instancer)
{
	this->position = position;
	this->scale = scale;
	this->instancer = instancer;

	this->instancer->instanceTransformMatrices.insert(pair<Base3DParticleInstance*, D3DXMATRIX>(this, world));
	this->instancer->instances.insert(pair<int, Base3DParticleInstance*>(GetID(), this));

	this->Update();
}

Base3DParticleInstance::~Base3DParticleInstance()
{
}

void Base3DParticleInstance::Update()
{
	D3DXMATRIX mS, mR, mT;
	D3DXMatrixScaling(&mS, scale.x, scale.y, scale.z);
	D3DXMatrixRotationQuaternion(&mR, &orientation);
	D3DXMatrixTranslation(&mT, position.x, position.y, position.z);

	world = mS * mR * mT;

	// Set the scale
	world._13 = scale.x;
	world._24 = scale.y;

	// Set the image, alpha and color mod
	world._12 = pMods.x;
	world._23 = pMods.y;
	world._34 = pMods.z;

	instancer->instanceTransformMatrices[this] = world;
}
