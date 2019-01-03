#include "Framework.h"
#include "Math.h"

const float Math::PI = 3.14159265f;
const float Math::EPSILON = 0.000001f;

float Math::Modulo(float val1, float val2)
{
	while (val1 - val2 >= 0)
		val1 -= val2;

	return val1;
}

float Math::ToRadian(float degree)
{
	return degree * PI / 180.0f;
}

float Math::ToDegree(float radian)
{
	return radian * 180.0f / PI;
}

float Math::Random(float r1, float r2)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = r2 - r1;
	float val = random * diff;
	
	return r1 + val;
}

int Math::Random(int r1, int r2)
{
	return (int)(rand() % (r2 - r1 + 1)) + r1;
}

float Math::Clamp(float value, float min, float max)
{
	value = value > max ? max : value;
	value = value < min ? min : value;

	return value;
}

void Math::GetDistance(OUT float & distance, const D3DXVECTOR3 v1, const D3DXVECTOR3 v2)
{
	distance = D3DXVec3Length(&(v2 - v1));
}

void Math::LerpMatrix(OUT D3DXMATRIX & out, const D3DXMATRIX & m1, const D3DXMATRIX & m2, float amount)
{
	out._11 = m1._11 + (m2._11 - m1._11) * amount;
	out._12 = m1._12 + (m2._12 - m1._12) * amount;
	out._13 = m1._13 + (m2._13 - m1._13) * amount;
	out._14 = m1._14 + (m2._14 - m1._14) * amount;

	out._21 = m1._21 + (m2._21 - m1._21) * amount;
	out._22 = m1._22 + (m2._22 - m1._22) * amount;
	out._23 = m1._23 + (m2._23 - m1._23) * amount;
	out._24 = m1._24 + (m2._24 - m1._24) * amount;

	out._31 = m1._31 + (m2._31 - m1._31) * amount;
	out._32 = m1._32 + (m2._32 - m1._32) * amount;
	out._33 = m1._33 + (m2._33 - m1._33) * amount;
	out._34 = m1._34 + (m2._34 - m1._34) * amount;

	out._41 = m1._41 + (m2._41 - m1._41) * amount;
	out._42 = m1._42 + (m2._42 - m1._42) * amount;
	out._43 = m1._43 + (m2._43 - m1._43) * amount;
	out._44 = m1._44 + (m2._44 - m1._44) * amount;
}

void Math::Lerp(OUT float & out, const float val1, const float val2, const float t)
{
	float _t = Clamp(t, 0.0f, 1.0f);

	out = val1 * (1.0f - t) + val2 * t;
}

void Math::LerpAngle(OUT float & out, float a, float b, float t)
{
	if (abs(a - b) >= D3DX_PI) {
		if (a > b)
			a = (float)(Math::NormalizeAngle(a) - 2.0f * D3DX_PI);
		else
			b = (float)(Math::NormalizeAngle(b) - 2.0f * D3DX_PI);
	}

	Math::Lerp(out, a, b, t);
}

float Math::NormalizeAngle(const float & x)
{
	return (float)(Math::FPosMod((float)(x + D3DX_PI), (float)(2.0f * D3DX_PI)) - D3DX_PI);
}

float Math::FPosMod(const float & x, const float & y)
{
	return (x >= 0) ? fmod(x, y) : y - fmod(-x, y);
}

void Math::MultiplyQuatVec3(OUT D3DXVECTOR3 & result, const D3DXQUATERNION & quat, const D3DXVECTOR3 & vec)
{
	float num = quat.x * 2.0f;
	float num2 = quat.y * 2.0f;
	float num3 = quat.z * 2.0f;
	float num4 = quat.x * num;
	float num5 = quat.y * num2;
	float num6 = quat.z * num3;
	float num7 = quat.x * num2;
	float num8 = quat.x * num3;
	float num9 = quat.y * num3;
	float num10 = quat.w * num;
	float num11 = quat.w * num2;
	float num12 = quat.w * num3;

	result.x = (1.0f - (num5 + num6)) * vec.x + (num7 - num12) * vec.y + (num8 + num11) * vec.z;
	result.y = (num7 + num12) * vec.x + (1.0f - (num4 + num6)) * vec.y + (num9 - num10) * vec.z;
	result.z = (num8 - num11) * vec.x + (num9 + num10) * vec.y + (1.0f - (num4 + num5)) * vec.z;
}

void Math::RotateVectorByQuaternion(OUT D3DXVECTOR3 & result, const D3DXVECTOR3 & v, const D3DXQUATERNION & q)
{
	// Extract the vector part of the quaternion
	D3DXVECTOR3 u(q.x, q.y, q.z);

	// Extract the scalar part of the quaternion
	float s = q.w;

	D3DXVECTOR3 temp;
	D3DXVec3Cross(&temp, &u, &v);

	result = 
		2.0f * D3DXVec3Dot(&u, &v) * u
		+ (s * s - D3DXVec3Dot(&u, &u)) * v
		+ 2.0f * s * temp;
}

void Math::QuatToPitchYawRoll(const D3DXQUATERNION & q, OUT float & pitch, OUT float & yaw, OUT float & roll, bool bToDegree)
{
	float sqw = q.w * q.w;
	float sqx = q.x * q.x;
	float sqy = q.y * q.y;
	float sqz = q.z * q.z;

	// rotation about x-axis
	pitch = asinf(2.0f * (q.w * q.x - q.y * q.z));
	// rotation about y-axis
	yaw = atan2f(2.0f * (q.x * q.z + q.w * q.y), (-sqx - sqy + sqz + sqw));
	// rotation about z-axis
	roll = atan2f(2.0f * (q.x * q.y + q.w * q.z), (-sqx + sqy - sqz + sqw));

	if (bToDegree)
	{
		pitch = Math::ToDegree(pitch);
		yaw = Math::ToDegree(yaw);
		roll = Math::ToDegree(roll);
	}
}

void Math::D3DXVec3Rotate(OUT D3DXVECTOR3 & result, const D3DXVECTOR3 & pV, const D3DXQUATERNION & pQ)
{
	D3DXQUATERNION q1, q2;
	D3DXQuaternionConjugate(&q1, &pQ);
	q2 = q1 * D3DXQUATERNION(pV.x, pV.y, pV.z, 1.0f) * pQ;

	result = D3DXVECTOR3(q2.x, q2.y, q2.z);
}

D3DXQUATERNION Math::LookAt(const D3DXVECTOR3 & origin, const D3DXVECTOR3 & target, const D3DXVECTOR3 & up)
{
	D3DXVECTOR3 forward = (origin - target);
	D3DXVec3Normalize(&forward, &forward);

	D3DXVECTOR3 right;
	D3DXVec3Cross(&right, &up, &forward);
	D3DXVec3Normalize(&right, &right);

	D3DXVECTOR3 _up;
	D3DXVec3Cross(&_up, &forward, &right);

	float m00 = right.x;
	float m01 = right.y;
	float m02 = right.z;
	float m10 = _up.x;
	float m11 = _up.y;
	float m12 = _up.z;
	float m20 = forward.x;
	float m21 = forward.y;
	float m22 = forward.z;

	float num8 = (m00 + m11) + m22;

	D3DXQUATERNION quat;

	if (num8 > EPSILON)
	{
		float num = (float)sqrt(num8 + 1.0f);
		quat.w = num * 0.5f;
		num = 0.5f / num;
		quat.x = (m12 - m21) * num;
		quat.y = (m20 - m02) * num;
		quat.z = (m01 - m10) * num;
	}
	else if ((m00 >= m11) && (m00 >= m22))
	{
		float num7 = (float)sqrt(((1.0f + m00) - m11) - m22);
		float num4 = 0.5f / num7;

		quat.x = 0.5f * num7;
		quat.y = (m01 + m10) * num4;
		quat.z = (m02 + m20) * num4;
		quat.w = (m12 - m21) * num4;
	}
	else if (m11 > m22)
	{
		float num6 = (float)sqrtf(((1.0f + m11) - m00) - m22);
		float num3 = 0.5f / num6;
		quat.x = (m10 + m01) * num3;
		quat.y = 0.5f * num6;
		quat.z = (m21 + m12) * num3;
		quat.w = (m20 - m02) * num3;
	}
	else
	{
		float num5 = (float)sqrtf(((1.0f + m22) - m00) - m11);
		float num2 = 0.5f / num5;
		quat.x = (m20 + m02) * num2;
		quat.y = (m21 + m12) * num2;
		quat.z = 0.5f * num5;
		quat.w = (m01 - m10) * num2;
	}

	return quat;
}

void Math::toEulerAngle(const D3DXQUATERNION & q, float & pitch, float & yaw, float & roll)
{
	float sqw = q.w*q.w;
    float sqx = q.x*q.x;
    float sqy = q.y*q.y;
    float sqz = q.z*q.z;
    pitch = asinf(2.0f * (q.w*q.x - q.y*q.z)); // rotation about x-axis
    yaw = atan2f(2.0f * (q.x*q.z + q.w*q.y), (-sqx - sqy + sqz + sqw)); // rotation about y-axis
    roll = atan2f(2.0f * (q.x*q.y + q.w*q.z), (-sqx + sqy - sqz + sqw)); // rotation about z-axis
}

void Math::toEulerAngle(const D3DXQUATERNION & q, D3DXVECTOR3 & out)
{
	toEulerAngle(q, out.x, out.y, out.z);
}

D3DXVECTOR3 Math::Transform(D3DXVECTOR3 value, D3DXQUATERNION rotation)
{
	D3DXVECTOR3 result;

	float x = 2 * (rotation.y * value.z - rotation.z * value.y);
	float y = 2 * (rotation.z * value.x - rotation.x * value.z);
	float z = 2 * (rotation.x * value.y - rotation.y * value.x);

	result.x = value.x + x * rotation.w + (rotation.y * z - rotation.z * y);
	result.y = value.y + y * rotation.w + (rotation.z * x - rotation.x * z);
	result.z = value.z + z * rotation.w + (rotation.x * y - rotation.y * x);

	return result;
}


D3DXMATRIX Math::CreateWorld(const D3DXVECTOR3 * position, const D3DXVECTOR3 * forward, const D3DXVECTOR3 * up)
{
	D3DXMATRIX mat;
	CreateWorld(position, forward, up, &mat);

	return mat;
}

void Math::CreateWorld(const D3DXVECTOR3 * position, const D3DXVECTOR3 * forward, const D3DXVECTOR3 * up, D3DXMATRIX * result)
{
	D3DXVECTOR3 x, y, z;
	D3DXVec3Normalize(&z, forward);
	D3DXVec3Cross(&x, up, forward);
	D3DXVec3Cross(&y, forward, &x);

	D3DXVec3Normalize(&x, &x);
	D3DXVec3Normalize(&y, &y);

	D3DXMatrixIdentity(result);
	result->_11 = x.x;
	result->_12 = x.y;
	result->_13 = x.z;

	result->_21 = y.x;
	result->_22 = y.y;
	result->_23 = y.z;

	result->_31 = z.x;
	result->_32 = z.y;
	result->_33 = z.z;

	result->_41 = position->x;
	result->_42 = position->y;
	result->_43 = position->z;
	result->_44 = 1.0f;
}

int Math::GetHashCode(int & val)
{
	int hash = 0;
	int tVal = val;

	do
	{
		hash = 31 * hash + tVal;
	} while ((tVal = tVal >> 1) != 0);

	return hash;
}

int Math::GetHashCode(string & val)
{
	int hash = 0, strlen = val.length(), i;
	char character;

	if (strlen == 0)
		return hash;

	for (i = 0; i < strlen; i++) {
		character = val.at(i);
		hash = (31 * hash) + ((int)character);
	}

	return hash;
}

int Math::GetHashCode(wstring & val)
{
	return GetHashCode(String::ToString(val));
}

float Math::DiagonalDistance2(Point start, Point goal)
{
	int dx = abs(start.X - goal.X);
	int dy = abs(start.Y - goal.Y);
	float h = (dx + dy) + (sqrtf(2) - 2) * min(dx, dy);

	return h;
}
