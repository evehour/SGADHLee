#pragma once

class Renderer
{
public:
	Renderer();
	virtual ~Renderer();

	void Enable(bool val);
	bool Enable();
	vector<function<void(bool)>> Enabled;

	void Visible(bool val);
	bool Visible();
	vector<function<void(bool)>> Visibled;

	D3DXMATRIX World();

	void Position(D3DXVECTOR3& vec);
	void Position(float x, float y, float z);
	D3DXVECTOR3 Position();

	void Scale(D3DXVECTOR3& vec);
	void Scale(float x, float y, float z);
	D3DXVECTOR3 Scale();

	virtual void Rotation(D3DXVECTOR3& vec);
	virtual void Rotation(float x, float y, float z);
	virtual void RotationDegree(D3DXVECTOR3& vec);
	virtual void RotationDegree(float x, float y, float z);
	D3DXVECTOR3 Rotation();
	D3DXVECTOR3 RotationDegree();


	D3DXVECTOR3 Direction();
	D3DXVECTOR3 Up();
	D3DXVECTOR3 Right();

	Material* GetMaterial() { return material; }

	virtual void Update() = 0;
	virtual void Render() = 0;

private:
	void UpdateWorld();

protected:
	bool enable;
	bool visible;

private:
	Material* material;
	D3DXMATRIX world;

	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 rotation;

	D3DXVECTOR3 direction;
	D3DXVECTOR3 up;
	D3DXVECTOR3 right;
};