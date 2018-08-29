#include "stdafx.h"
#include "AreaLight.h"

AreaLight::AreaLight()
{
	buffer = new Buffer();
}

AreaLight::~AreaLight()
{
	SAFE_DELETE(buffer);
}

void AreaLight::Update()
{
	buffer->SetPSBuffer(4);
}
