#pragma once
class Math
{
public:
	static const float PI;
	static const float EPSILON;

	static float Modulo(float val1, float val2);

	static float ToRadian(float degree);
	static float ToDegree(float radian);

	static int Random(int r1, int r2);
	static float Random(float r1, float r2);

	static float Clamp(float value, float min, float max);

	static void GetDistance(OUT float & distance, const D3DXVECTOR3 v1, const D3DXVECTOR3 v2);

	static void LerpMatrix(OUT D3DXMATRIX& out, const D3DXMATRIX& m1, const D3DXMATRIX& m2, float amount);
	static void Lerp(OUT float & out, const float val1, const float val2, const float t);
	static void LerpAngle(OUT float & out, float a, float b, float t);
	static float NormalizeAngle(const float & x);
	static float FPosMod(const float & x, const float & y);

	static void MultiplyQuatVec3(OUT D3DXVECTOR3 & result, const D3DXQUATERNION & quat, const D3DXVECTOR3 & vec);

	static void QuatToPitchYawRoll(const D3DXQUATERNION& q,
		OUT float& pitch, OUT float& yaw, OUT float& roll, bool bToDegree = false);
	static void D3DXVec3Rotate(OUT D3DXVECTOR3 & result, const D3DXVECTOR3 &pV, const D3DXQUATERNION &pQ);
	static void RotateVectorByQuaternion(OUT D3DXVECTOR3 & result, const D3DXVECTOR3 & v, const D3DXQUATERNION & q);


	static D3DXQUATERNION LookAt(const D3DXVECTOR3& origin, const D3DXVECTOR3& target, const D3DXVECTOR3& up);

	static void toEulerAngle(const D3DXQUATERNION& q, float& pitch, float& yaw, float& roll);
	static void toEulerAngle(const D3DXQUATERNION& q, D3DXVECTOR3& out);

	///////////////////////////////////////////////////////////////////////////

	static D3DXVECTOR3 Transform(D3DXVECTOR3 value, D3DXQUATERNION rotation);

	static D3DXMATRIX CreateWorld(const D3DXVECTOR3* position, const D3DXVECTOR3* forward, const D3DXVECTOR3* up);
	static void CreateWorld(const D3DXVECTOR3* position, const D3DXVECTOR3* forward, const D3DXVECTOR3* up, D3DXMATRIX* result);
	
	///////////////////////////////////////////////////////////////////////////

	static int GetHashCode(int & val);
	static int GetHashCode(string & val);
	static int GetHashCode(wstring & val);

	//-----------------------------------------------------------------------------
	static float DiagonalDistance2(Point start, Point goal);
};