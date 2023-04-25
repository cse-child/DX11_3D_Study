#pragma once

class Converter
{
public:
	Converter();
	~Converter();

	void ReadFile(wstring file);

public:
	void ExportMesh(wstring savePath);

private:
	void ReadBoneData(aiNode* node, int index, int parent);
	void ReadMeshData(aiNode* node, int bone); // bone: Mesh가 참조할 부모
	// Mesh와 Bone을 원하는 형태로 불러온 뒤 원하는 형태로 저장하여 사용
	void WriteMeshData(wstring savePath);

private:
	wstring file;

	Assimp::Importer* importer;
	const aiScene* scene;

	vector<struct asBone*> bones; // 전방선언
	vector<struct asMesh*> meshes;
};