#pragma once

class Base3DParticleInstance
{
public:
	Base3DParticleInstance();
	Base3DParticleInstance(D3DXVECTOR3 position, D3DXVECTOR3 scale, class Base3DParticleInstancer* instancer, D3DXQUATERNION orientation);
	Base3DParticleInstance(D3DXVECTOR3 position, D3DXVECTOR3 scale, D3DXVECTOR3 mods, class Base3DParticleInstancer* instancer);
	Base3DParticleInstance(D3DXVECTOR3 position, D3DXVECTOR3 scale, class Base3DParticleInstancer* instancer);
	~Base3DParticleInstance();

	void Update();

	int GetID() { return this->id; }
private:
	static int ID;
	int id;

	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXQUATERNION orientation;

	D3DXVECTOR3 pMods;

	D3DXMATRIX world;

	class Base3DParticleInstancer* instancer;
};