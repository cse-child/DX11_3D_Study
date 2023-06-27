#include "Framework.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(Shader* shader)
{
	model = new Model();
	transform = new Transform(shader);
}

ModelAnimator::~ModelAnimator()
{
	SafeDelete(model);
	SafeDelete(transform);
}

void ModelAnimator::Update()
{
	if(texture == NULL)
	{
		//for (ModelMesh* mesh : model->Meshes())
		//	mesh->SetShader(shader);

		CreateTexture();
	}
	for (ModelMesh* mesh : model->Meshes())
		mesh->Update();
}

void ModelAnimator::Render()
{
	for (ModelMesh* mesh : model->Meshes())
	{
		mesh->SetTransform(transform);
		mesh->Render();
	}
}

void ModelAnimator::ReadMesh(wstring file)
{
	model->ReadMesh(file);
}

void ModelAnimator::ReadMaterial(wstring file)
{
	model->ReadMaterial(file);
}

void ModelAnimator::ReadClip(wstring file)
{
	model->ReadClip(file);
}

void ModelAnimator::Pass(UINT pass)
{
	for (ModelMesh* mesh : model->Meshes())
		mesh->Pass(pass);
}

void ModelAnimator::CreateTexture()
{
	clipTransforms = new ClipTransform[model->ClipCount()];
	for(UINT i = 0; i < model->ClipCount(); i++)
		CreateClipTransform(i);

	/* Create Texture */
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_MODEL_KEYFRAMES * 4;				// Texture의 넓이에는 Bone 값이 들어간다.
		desc.Height = MAX_MODEL_KEYFRAMES;
		desc.ArraySize = model->ClipCount();
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;		// 16Byte * 4 = 64Byte (쪼개서 다시 가져오는건 Shader에서 처리)
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		// 한 면의 크기는 Transform에 픽셀 하나 당 16Byte가 4개씩 들어가고, 행렬이니 16을 곱하면 된다.
		// pageSize = 250(Transforms) * 4 * 16 * 500(Keyframes) = 8,000,000 (8메가) -> malloc으로 복사 불가능 + 스택오버플로우
		UINT pageSize = MAX_MODEL_TRANSFORMS * 4 * 16 * MAX_MODEL_KEYFRAMES;
		//void* p = malloc(pageSize * model->ClipCount());
		void* p = VirtualAlloc(NULL, pageSize * model->ClipCount(), MEM_RESERVE, PAGE_READWRITE);

		// MEMORY_BASIC_INFORMATION, VirtualQuery : 실제로 예약한 사이즈를 알 수 있다.

		for(UINT c = 0; c < model->ClipCount(); c++)
		{
			UINT start = c * pageSize;

			for(UINT k = 0; k < MAX_MODEL_KEYFRAMES; k++)
			{
				void* temp = (BYTE*)p + MAX_MODEL_TRANSFORMS * k * sizeof(Matrix) + start; // (BYTE*)를 void*로 정의하여 1바이트 넘기도록 설정

				VirtualAlloc(temp, MAX_MODEL_TRANSFORMS * sizeof(Matrix), MEM_COMMIT, PAGE_READWRITE);
				memcpy(temp, clipTransforms[c].Transform[k], MAX_MODEL_TRANSFORMS * sizeof(Matrix));
			}
		}// for(c)
		
		D3D11_SUBRESOURCE_DATA* subResources = new D3D11_SUBRESOURCE_DATA[model->ClipCount()];
		for(int c = 0; c < model->ClipCount(); c++)
		{
			void* temp = (BYTE*)p + c * pageSize;

			subResources[c].pSysMem = temp;
			subResources[c].SysMemPitch = MAX_MODEL_TRANSFORMS * sizeof(Matrix);
			subResources[c].SysMemSlicePitch = pageSize;
		}
		Check(D3D::GetDevice()->CreateTexture2D(&desc, subResources, &texture));

		SafeDeleteArray(subResources);
		VirtualFree(p, 0, MEM_RELEASE);
	}
}

void ModelAnimator::CreateClipTransform(UINT index)
{
	Matrix* bones = new Matrix[MAX_MODEL_TRANSFORMS];

	ModelClip* clip = model->ClipByIndex(index);
	for(UINT f = 0; f < clip->FrameCount(); f++)
	{
		for(UINT b = 0; b < model->BoneCount(); b++)
		{
			ModelBone* bone = model->BoneByIndex(b);

			Matrix parent;
			Matrix invGlobal = bone->Transform();
			D3DXMatrixInverse(&invGlobal, NULL, &invGlobal); // invGlobal 변수 역행렬 취하기

			int parentIndex = bone->ParentIndex();
			if (parentIndex < 0)
				D3DXMatrixIdentity(&parent);
			else
				parent = bones[parentIndex];

			Matrix animation;
			ModelKeyframe* frame = clip->Keyframe(bone->Name());

			/* Animation Bone이 프레임에 따라 얼마만큼 이동할지 설정 */
			if(frame != NULL)
			{
				ModelKeyframeData& data = frame->Transforms[f];

				Matrix S, R, T;
				D3DXMatrixScaling(&S, data.Scale.x, data.Scale.y, data.Scale.z);
				D3DXMatrixRotationQuaternion(&R, &data.Rotation);
				D3DXMatrixTranslation(&T, data.Translation.x, data.Translation.y, data.Translation.z);

				animation = S * R  * T;
			}
			else
			{
				D3DXMatrixIdentity(&animation);
			}

			bones[b] = animation * parent;
			clipTransforms[index].Transform[f][b] = invGlobal * bones[b];
		} // for(b)
	} // for(f)
}


