#pragma once

class Mesh
{
public:
	typedef VertexTextureNormal MeshVertex;

public:
	Mesh(Shader* shader);
	virtual ~Mesh();

	void Update();
	void Render();

public:
	void Pass(UINT val) { pass = val; }

	void Position(float x, float y, float z);
	void Position(Vector3& vec);
	void Position(Vector3* vec);

	void Rotation(float x, float y, float z);
	void Rotation(Vector3& vec);
	void Rotation(Vector3* vec);

	void RotationDegree(float x, float y, float z);
	void RotationDegree(Vector3& vec);
	void RotationDegree(Vector3* vec);

	void Scale(float x, float y, float z);
	void Scale(Vector3& vec);
	void Scale(Vector3* vec);

	Matrix World() { return world; }

	Vector3 Forward();
	Vector3 Up();
	Vector3 Right();

	void DiffuseMap(wstring file);

protected:
	virtual void Create() = 0;
	void CreateBuffer();

private:
	void UpdateWorld();

protected:
	MeshVertex* vertices = NULL;
	UINT* indices = NULL;

	UINT vertexCount;
	UINT indexCount;

private:
	Shader* shader;
	UINT pass = 0;

	Vector3 position = Vector3(0, 0, 0);
	Vector3 scale = Vector3(1, 1, 1);
	Vector3 rotation = Vector3(0, 0, 0);

	Matrix world;

	ID3D11Buffer* vertexBuffer = NULL;
	ID3D11Buffer* indexBuffer = NULL;

	// as matrix, as vector, as scalar로 값을 가져와서 쉐이더에 전달하면
	// 변수를 찾아주는 as 함수가 쉐이더안에 있는 변수에서 검색해야하기 때문에 속도가 느리다.
	// 따라서 미리 검색해서 찾아놓으면 속도가 빠르다.
	ID3DX11EffectMatrixVariable* sWorld, *sView, *sProjection;

	Texture* diffuseMap = NULL;
	ID3DX11EffectShaderResourceVariable* sDiffuseMap;
};