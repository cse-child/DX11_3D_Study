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
	// aiProcess_ConvertToLeftHanded �޼� ��ǥ��
	// aiProcess_Triangulate : �ﰢ���� �������� ��ȯ�Ͽ� ��ȯ
	// aiProcess_GenUVCoords : �ﰢ������ ��ȯ�ƴٸ� UV ��ǥ�� ���⿡ �°� ��ȯ�϶�
	// aiProcess_GenNormals : �ﰢ������ ��ȯ�ƴٸ� Normal�� ���⿡ �°� ��ȯ�϶�
	// aiProcess_CalcTangentSpace : Normal ���ο� ���Ǵ� ź��Ʈ �����̽�
	assert(scene != NULL);
}

void Converter::ExportMesh(wstring savePath)
{
	savePath = L"../../_Models/" + savePath + L".mesh";

	// RootNode ���� ��ͷ� ������ ��������
	ReadBoneData(scene->mRootNode, -1, -1);
}

void Converter::ReadBoneData(aiNode* node, int index, int parent)
{
	// TODO: �� ���� �б�
	asBone* bone = new asBone();
	bone->Index = index;
	bone->Parent = parent;
	bone->Name = node->mName.C_Str();

	Matrix transform(node->mTransformation[0]); // ���� �ּҸ� ���� == Trasform ����
	D3DXMatrixTranspose(&bone->Transform, &transform); // �ϳ��� �ʱ�ȭ�� Transform�� ��ü Transform���� Ȯ��

	Matrix matParent;
	if (parent < 0) // parent���� ������� �ش� node�� root�� �� == �θ� ���� ��
		D3DXMatrixIdentity(&matParent);
	else
		matParent = bones[parent]->Transform;

	bone->Transform = bone->Transform * matParent;
	bones.push_back(bone);

	// TODO: �޽� ���� �б�

	ReadMeshData(node, index); // �� ��尡 �������ִ� Mesh ���� �ҷ�����

	for (UINT i = 0; i < node->mNumChildren; i++)
		ReadBoneData(node->mChildren[i], bones.size(), index);
}

void Converter::ReadMeshData(aiNode* node, int bone)
{
}

void Converter::WriteMeshData(wstring savePath)
{
}



