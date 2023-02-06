#include "stdafx.h"
#include "Vertex_Line2.h"

void Vertex_Line2::Initialize()
{
	shader = new Shader(L"04_Vertex.fx");

	vertices[0].Position = Vector3(0.0f, 0.0f, 0.0f);
	vertices[1].Position = Vector3(1.0f, 0.0f, 0.0f);

	vertices[2].Position = Vector3(0.0f, 0.5f, 0.0f);
	vertices[3].Position = Vector3(1.0f, 0.5f, 0.0f);

	vertices[4].Position = Vector3(0.0f, -0.5f, 0.0f);
	vertices[5].Position = Vector3(1.0f, -0.5f, 0.0f);

	// 뭔가를 그릴 수 있도록 CPU로 데이터를 넘겨주는 것
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(Vertex) * 6; // 그려질 자료형 크기
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 그려질 용도

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = vertices;// 정점을 그릴 데이터의 시작주소

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
}

void Vertex_Line2::Destroy()
{
	SafeDelete(shader); // new로 생성한 것은 Delete로 지운다
	SafeRelease(vertexBuffer); // I자가 붙은 것은 Release로 지운다
}

void Vertex_Line2::Update()
{
}

void Vertex_Line2::Render()
{
	UINT stride = sizeof(Vertex); // 정점 하나의 크기
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	//D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	shader->Draw(0, 0, 6);
	//shader->Draw(1, 0, 2, 2); // 맨 뒤 두개: 몇개를 그릴지, 시작할 번호
}