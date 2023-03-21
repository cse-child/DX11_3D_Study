#include "stdafx.h"
#include "Converter.h"
#include "Types.h"

Converter::Converter()
{
	importer = new Assimp::Importer();
}

Converter::~Converter()
{
	SafeDelete(importer);
}

void Converter::ReadFile()
{
	this->file = L"../../_Assets/" + file;

	scene = importer->ReadFile
	(
		String::ToString(this->file),
		aiProcess_ConvertToLeftHanded
		| aiProcess_Triangulate
		| aiProcess_GenUVCoords
		| aiProcess_GenNormals
		| aiProcess_CalcTangentSpace
	);
	// aiProcess_ConvertToLeftHanded 왼손 좌표계
	// aiProcess_Triangulate : 삼각형을 기준으로 변환하여 반환
	// aiProcess_GenUVCoords : 삼각형으로 변환됐다면 UV 좌표를 여기에 맞게 변환하라
	// aiProcess_GenNormals : 삼각형으로 변환됐다면 Normal을 여기에 맞게 반환하라
	// aiProcess_CalcTangentSpace : Normal 맵핑에 사용되는 탄젠트 스페이스
	assert(scene != NULL);
}

void Converter::ExportMesh(wstring savePath)
{
	savePath = L"../../_Models/" + savePath + L".mesh";

	// RootNode 부터 재귀로 데이터 가져오기
	ReadBoneData(scene->mRootNode, -1, -1);
}

void Converter::ReadBoneData(aiNode* node, int index, int parent)
{
	// TODO: 본 정보 읽기
	asBone* bone = new asBone();
	bone->Index = index;
	bone->Parent = parent;
	bone->Name = node->mName.C_Str();

	Matrix transform(node->mTransformation[0]); // 시작 주소를 전달 == Trasform 복사
	D3DXMatrixTranspose(&bone->Transform, &transform); // 하나씩 초기화한 Transform을 전체 Transform으로 확정

	Matrix matParent;
	if (parent < 0) // parent값이 음수라면 해당 node가 root인 것 == 부모가 없는 것
		D3DXMatrixIdentity(&matParent);
	else
		matParent = bones[parent]->Transform;

	bone->Transform = bone->Transform * matParent;
	bones.push_back(bone);

	// TODO: 메시 정보 읽기

	ReadMeshData(node, index); // 각 노드가 가지고있는 Mesh 정보 불러오기

	for (UINT i = 0; i < node->mNumChildren; i++)
		ReadBoneData(node->mChildren[i], bones.size(), index);
}

void Converter::ReadMeshData(aiNode* node, int bone)
{
}

void Converter::WriteMeshData(wstring savePath)
{
}



