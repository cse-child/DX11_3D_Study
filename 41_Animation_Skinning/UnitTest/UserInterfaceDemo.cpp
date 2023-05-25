#include "stdafx.h"
#include "UserInterfaceDemo.h"

void UserInterfaceDemo::Initialize()
{
	shader = new Shader(L"07_Rect.fx");

	vertices[0].Position = Vector3(+0.0f, +0.0f, 0.0f);
	vertices[1].Position = Vector3(+0.0f, +0.5f, 0.0f);
	vertices[2].Position = Vector3(+0.5f, +0.0f, 0.0f);

	vertices[3].Position = Vector3(+0.5f, +0.0f, 0.0f);
	vertices[4].Position = Vector3(+0.0f, +0.5f, 0.0f);
	vertices[5].Position = Vector3(+0.5f, +0.5f, 0.0f);

	// ������ �׸� �� �ֵ��� CPU�� �����͸� �Ѱ��ִ� �� (IA�� �� Vertex ����)
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(Vertex) * 6; // �׷��� �ڷ��� ũ��
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // �׷��� �뵵

	// ������ GPU�� �������ִ� �κ�
	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = vertices;// ������ �׸� �������� �����ּ�

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
}

void UserInterfaceDemo::Destroy()
{
	SafeDelete(shader); // new�� ������ ���� Delete�� �����

	SafeRelease(vertexBuffer); // I�ڰ� ���� ���� Release�� �����
}

void UserInterfaceDemo::Update()
{
	static bool bOpen = true;
	//ImGui::Checkbox("Show Demo Window", &bOpen);
	//if (ImGui::Button("Open"))
	//	bOpen = !bOpen;

	if (Keyboard::Get()->Down(VK_SPACE))
		bOpen = !bOpen;
	
	if(bOpen)
		ImGui::ShowDemoWindow(&bOpen);

	if (Keyboard::Get()->Press(VK_RIGHT))
		vertices[0].Position.x += 2.0f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_LEFT))
		vertices[0].Position.x -= 2.0f * Time::Delta();

	static float y = 0.5f;
	ImGui::SliderFloat("Y", &y, -1, +1);
	//ImGui::InputFloat("Y", &y, 0.01f);

	vertices[1].Position.y = y;
	
	D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, vertices, sizeof(Vertex) * 6, 0);
}

void UserInterfaceDemo::Render()
{
	string text = "";
	if (Keyboard::Get()->Press(VK_RIGHT))
		text += "Right";
	else if (Keyboard::Get()->Press(VK_LEFT))
		text += "Left";

	Gui::Get()->RenderText(10, 60, 1, 0, 0, text);

	UINT stride = sizeof(Vertex); // ���� �ϳ��� ũ��
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->Draw(0, 0, 6);
}