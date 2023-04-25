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
	void ReadMeshData(aiNode* node, int bone); // bone: Mesh�� ������ �θ�
	// Mesh�� Bone�� ���ϴ� ���·� �ҷ��� �� ���ϴ� ���·� �����Ͽ� ���
	void WriteMeshData(wstring savePath);

private:
	wstring file;

	Assimp::Importer* importer;
	const aiScene* scene;

	vector<struct asBone*> bones; // ���漱��
	vector<struct asMesh*> meshes;
};