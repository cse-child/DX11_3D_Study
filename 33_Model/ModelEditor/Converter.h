#pragma once

class Converter
{
public:
	Converter();
	~Converter();

	void ReadFile();

private:
	wstring file;

	Assimp::Importer* importer;
	const aiScene* scene;
};