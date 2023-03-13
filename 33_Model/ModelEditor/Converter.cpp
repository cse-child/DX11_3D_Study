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
	// aiProcess_ConvertToLeftHanded 왼손 좌표계
	// aiProcess_Triangulate : 삼각형을 기준으로 변환하여 반환
	// aiProcess_GenUVCoords : 삼각형으로 변환됐다면 UV 좌표를 여기에 맞게 변환하라
	// aiProcess_GenNormals : 삼각형으로 변환됐다면 Normal을 여기에 맞게 반환하라
	// aiProcess_CalcTangentSpace : Normal 맵핑에 사용되는 탄젠트 스페이스
	assert(scene != NULL);
}



