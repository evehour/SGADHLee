#include "Framework.h"
#include "CBuffer.h"

CBuffer::CBuffer(Shader * shader, string bufferName, void * pData, UINT dataSize)
	: shader(shader), name(bufferName), data(pData), dataSize(dataSize)
{
	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = dataSize;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &buffer);
	assert(SUCCEEDED(hr));

	Change();

	CBuffers::buffers.push_back(this);
}

CBuffer::~CBuffer()
{
	CBuffers::Erase(this);

	SAFE_RELEASE(buffer);
}

void CBuffer::Change()
{
	bChanged = true;
}

void CBuffer::SelfChange()
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	HRESULT hr = D3D::GetDC()->Map
	(
		buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
	);
	assert(SUCCEEDED(hr));

	memcpy(subResource.pData, data, dataSize);
	D3D::GetDC()->Unmap(buffer, 0);

	hr = shader->AsConstantBuffer(name)->SetConstantBuffer(buffer);
	assert(SUCCEEDED(hr));
}

void CBuffer::Changed()
{
	if (bChanged == true)
	{
		D3D11_MAPPED_SUBRESOURCE subResource;
		HRESULT hr = D3D::GetDC()->Map
		(
			buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
		);
		assert(SUCCEEDED(hr));

		memcpy(subResource.pData, data, dataSize);
		D3D::GetDC()->Unmap(buffer, 0);

		hr = shader->AsConstantBuffer(name)->SetConstantBuffer(buffer);
		assert(SUCCEEDED(hr));

		bChanged = false;
	}
}

///////////////////////////////////////////////////////////////////////////////

vector<CBuffer *> CBuffers::buffers;

void CBuffers::Update()
{
	for (CBuffer* buffer : buffers)
		buffer->Changed();
}

void CBuffers::Erase(CBuffer * buffer)
{
	for (vector<CBuffer *>::iterator iter = buffers.begin(); iter != buffers.end(); ++iter)
	{
		if ((*iter) == buffer)
		{
			//SAFE_DELETE((*iter));
			iter = buffers.erase(iter);
			break;
		}
	}
}