#include "stdafx.h"
#include "TriangleList.h"

void TriangleList::Initialize()
{
	shader = new Shader(L"06_Triangle.fx");

	// VertexBuffer
	{
		vertices[0].Position = Vector3(-0.5f, +0.0f, 0.0f);
		vertices[1].Position = Vector3(+0.0f, +0.5f, 0.0f);
		vertices[2].Position = Vector3(+0.5f, +0.0f, 0.0f);

		// 뭔가를 그릴 수 있도록 CPU로 데이터를 넘겨주는 것 (IA로 들어갈 Vertex 정보)
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * 3; // 그려질 자료형 크기
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 그려질 용도

		// 실제로 GPU를 복사해주는 부분
		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;// 정점을 그릴 데이터의 시작주소

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}

	// VertexBuffer2
	{
		vertices2[0].Position = Vector3(-0.5f, -0.5f, 0.0f);
		vertices2[1].Position = Vector3(+0.0f, -0.0f, 0.0f);
		vertices2[2].Position = Vector3(+0.5f, -0.5f, 0.0f);

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * 3;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices2;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer2));
	}
}

void TriangleList::Destroy()
{
	SafeDelete(shader); // new로 생성한 것은 Delete로 지운다

	SafeRelease(vertexBuffer); // I자가 붙은 것은 Release로 지운다
	SafeRelease(vertexBuffer2);
}

void TriangleList::Update()
{
}

void TriangleList::Render()
{
	UINT stride = sizeof(Vertex); // 정점 하나의 크기
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->Draw(0, 0, 3);

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer2, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->Draw(0, 1, 3);
}