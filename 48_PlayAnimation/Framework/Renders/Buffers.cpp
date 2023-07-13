#include "Framework.h"
#include "Buffers.h"

VertexBuffer::VertexBuffer(void * data, UINT count, UINT stride, UINT slot, bool bCpuWrite, bool bGpuWrite)
	: data(data), count(count), stride(stride), slot(slot)
	, bCpuWrite(bCpuWrite), bGpuWrite(bGpuWrite)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = stride * count;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	// desc.Usage : CPU에서 접근할수있느냐, GPU에서 접근할수있느냐를 알려줌
	if (bCpuWrite == false && bGpuWrite == false)
	{
		// 렌더링용으로만 사용하므로 속도가 가장 빠르다
		// IMMUTABLE : GPU 읽기만 가능(=렌더링 용으로만 접근)
		desc.Usage = D3D11_USAGE_IMMUTABLE;
	}
	else if (bCpuWrite == true && bGpuWrite == false)
	{
		// IMMUTABLE 보다는 느리다

		desc.Usage = D3D11_USAGE_DYNAMIC; // CPU 쓰기/GPU 읽기만
		desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	}
	else if (bCpuWrite == false && bGpuWrite == true)
	{
		// Default : CPU는 원래 쓰기가 불가능하고 읽기만 가능, GPU는 읽기쓰기 둘다 가능
		// 예외적으로 CPU 쓰기가 가능한 상황 : UpdateSubResource 함수 사용 시
		desc.Usage = D3D11_USAGE_DEFAULT;
	}
	else
	{
		// STAGING : 읽고 쓰기 전부 가능
		desc.Usage = D3D11_USAGE_STAGING;
		// D3D11_CPU_ACCESS_READ: GPU->CPU 보내기
		// D3D11_CPU_ACCESS_WRITE: CPU->GPU 보내기
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	}

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = data;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &buffer));
}

VertexBuffer::~VertexBuffer()
{
	SafeRelease(buffer);
}

void VertexBuffer::Render()
{
	UINT offset = 0;
	D3D::GetDC()->IASetVertexBuffers(slot, 1, &buffer, &stride, &offset);
}

///////////////////////////////////////////////////////////////////////////////

IndexBuffer::IndexBuffer(void * data, UINT count)
	: data(data), count(count)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.ByteWidth = sizeof(UINT) * count;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	// Index 버퍼는 한번 들어가면 절대 바뀔일이 없으므로 IMMUTABLE
	desc.Usage = D3D11_USAGE_IMMUTABLE;


	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = data;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &buffer));
}

IndexBuffer::~IndexBuffer()
{
	SafeRelease(buffer);
}

void IndexBuffer::Render()
{
	D3D::GetDC()->IASetIndexBuffer(buffer, DXGI_FORMAT_R32_UINT, 0);
}

///////////////////////////////////////////////////////////////////////////////


ConstantBuffer::ConstantBuffer(void * data, UINT dataSize)
	:data(data), dataSize(dataSize)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.ByteWidth = dataSize;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

	Check(D3D::GetDevice()->CreateBuffer(&desc, NULL, &buffer));
}

ConstantBuffer::~ConstantBuffer()
{
	SafeRelease(buffer);
}

void ConstantBuffer::Render()
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, data, dataSize);
	}
	// Map을 하면 반드시 Unmap 해야 한다.
	D3D::GetDC()->Unmap(buffer, 0);
}
