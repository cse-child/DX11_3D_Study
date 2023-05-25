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
	void ReadSkinData();

	// Mesh와 Bone을 원하는 형태로 불러온 뒤 원하는 형태로 저장하여 사용
	void WriteMeshData(wstring savePath);

public:
	void ExportMaterial(wstring savePath, bool bOverwrite = true);

private:
	void ReadMaterialData();
	void WriteMaterialData(wstring savePath);
	string WriteTexture(string saveFolder, string file);

private:
	wstring file;

	Assimp::Importer* importer;
	const aiScene* scene;

	vector<struct asBone*> bones; // 전방선언
	vector<struct asMesh*> meshes;
	vector<struct asMaterial*> materials;
};