#include "Framework.h"
#include "CubeSky.h"

CubeSky::CubeSky(wstring file)
{
	shader = new Shader(L"29_CubeSky.fx");

	sphere = new MeshSphere(shader, 0.5f);

	file = L"../../_Textures/" + file;
	Check(D3DX11CreateShaderResourceViewFromFile
	(
		D3D::GetDevice(), file.c_str(), NULL, NULL, &srv, NULL
	));

	sSrv = shader->AsSRV("SkyCubeMap");

}

CubeSky::~CubeSky()
{
	SafeDelete(shader);
	SafeDelete(sphere);

	SafeRelease(srv);
}

void CubeSky::Update()
{
	// 카메라의 위치로 구의 위치를 지정
	Vector3 position;
	Context::Get()->GetCamera()->Position(&position);

	sphere->Position(position);
}

void CubeSky::Render()
{
	sSrv->SetResource(srv);
	sphere->Render();
}
