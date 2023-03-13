#include "stdafx.h"
#include "Converter.h"

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



