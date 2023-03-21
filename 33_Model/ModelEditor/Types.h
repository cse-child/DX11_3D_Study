#pragma once
#include "stdafx.h"

struct asBone
{
	int Index;			// Bone Index
	string Name;		// Bone Name

	int Parent;			// Parent Index (null == -1)
	Matrix Transform;	// Parent-Child Matrix
};

struct asMesh
{
	string Name;		// Equal Bone Name
	int BoneIndex;

	aiMesh* Mesh;

	string MaterialName;

	vector<Model::ModelVertex> Vertices;
	vector<UINT> Indices;
};