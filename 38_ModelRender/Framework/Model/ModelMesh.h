#pragma once

class ModelBone
{
public:
	friend class Model;

private:
	ModelBone();
	~ModelBone();

public:
	int Index() { return index; }

	int ParentIndex() { return parentIndex; }
	ModelBone* Parent() { return parent; }

	wstring Name() { return name; }

	Matrix& Transform() { return transform; }
	void Transform(Matrix& matrix) { transform = matrix; }

	vector<ModelBone *>& Childs() { return childs; }

private:
	int index;
	wstring name;

	int parentIndex;
	ModelBone* parent;

	Matrix transform;
	vector<ModelBone*> childs;
};

///////////////////////////////////////////////////////////////////////////////

class ModelMesh
{
public:
	friend class Model;

private:
	ModelMesh();
	~ModelMesh();

	void Binding(Model* model);

public:
	void Pass(UINT val) { pass = val; }
	void SetShader(Shader* shader);

	void Update();
	void Render();

	wstring Name() { return name; }

	int BoneIndex() { return boneIndex; }
	class ModelBone* Bone() { return bone; }

	void Transforms(Matrix* transforms); // BoneDesc의 Transforms에 접근하기 위한 함수
	void SetTransform(Transform* transform); // Model의 위치를 움직이기 위한 함수

private:
	struct BoneDesc
	{
		Matrix Transforms[MAX_MODEL_TRANSFORMS];

		UINT index;
		float Padding[3];
	} boneDesc;

private:
	wstring name;

	Shader* shader;
	UINT pass = 0;

	Transform* transform = NULL;
	PerFrame* perFrame = NULL;

	wstring materialName = L"";


	int boneIndex;
	class ModelBone* bone;

	VertexBuffer* vertexBuffer;
	UINT vertexCount;
	Model::ModelVertex* vertices;

	IndexBuffer* indexBuffer;
	UINT indexCount;
	UINT* indices;

	ConstantBuffer* boneBuffer;
	ID3DX11EffectConstantBuffer* sBoneBuffer;
};

