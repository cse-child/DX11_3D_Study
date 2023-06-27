#include "stdafx.h"
#include "Converter.h"
#include "Types.h"
#include "Utilities/BinaryFile.h"
#include "Utilities/Xml.h"

Converter::Converter()
{
	importer = new Assimp::Importer();
}

Converter::~Converter()
{
	SafeDelete(importer);
}

void Converter::ReadFile(wstring file)
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

void Converter::ExportMesh(wstring savePath)
{
	savePath = L"../../_Models/" + savePath + L".mesh";

	// RootNode 부터 재귀로 데이터 가져오기
	ReadBoneData(scene->mRootNode, -1, -1);
	ReadSkinData();

	//Write CSV File
	{
		FILE* file;
		fopen_s(&file, "../Vertices.csv", "w");

		for(asBone* bone : bones)
		{
			string name = bone->Name;
			fprintf(file, "%d,%s\n", bone->Index, name.c_str());
		}
		fprintf(file, "\n");
		for(asMesh* mesh : meshes)
		{
			string name = mesh->Name;
			printf("%s\n", name.c_str());

			for(UINT i = 0; i < mesh->Vertices.size(); i++)
			{
				Vector3 p = mesh->Vertices[i].Position;
				Vector4 indices = mesh->Vertices[i].BlendIndices;
				Vector4 weights = mesh->Vertices[i].BlendWeights;

				fprintf(file, "%f,%f,%f,", p.x, p.y, p.z);
				fprintf(file, "%f,%f,%f,%f,", indices.x, indices.y, indices.z, indices.w);
				fprintf(file, "%f,%f,%f,%f\n", weights.x, weights.y, weights.z, weights.w);
			}
		}

		fclose(file);
	}

	WriteMeshData(savePath);
}

void Converter::ReadBoneData(aiNode* node, int index, int parent)
{
	// TODO: 본 정보 읽기
	asBone* bone = new asBone();
	bone->Index = index;
	bone->Parent = parent;
	bone->Name = node->mName.C_Str();

	Matrix transform(node->mTransformation[0]); // 시작 주소를 전달 == Trasform 복사
	D3DXMatrixTranspose(&bone->Transform, &transform); // 하나씩 초기화한 Transform을 전체 Transform으로 확정

	Matrix matParent;
	if (parent < 0) // parent값이 음수라면 해당 node가 root인 것 == 부모가 없는 것
		D3DXMatrixIdentity(&matParent);
	else
		matParent = bones[parent]->Transform;

	bone->Transform = bone->Transform * matParent;
	bones.push_back(bone);

	// TODO: 메시 정보 읽기

	ReadMeshData(node, index); // 각 노드가 가지고있는 Mesh 정보 불러오기

	for (UINT i = 0; i < node->mNumChildren; i++)
		ReadBoneData(node->mChildren[i], bones.size(), index);
}

void Converter::ReadMeshData(aiNode* node, int bone)
{
	if (node->mNumMeshes < 1) return;

	asMesh* mesh = new asMesh();
	mesh->Name = node->mName.C_Str();
	mesh->BoneIndex = bone;

	for(UINT i = 0; i < node->mNumMeshes; i++)
	{
		UINT index = node->mMeshes[i];
		aiMesh* srcMesh = scene->mMeshes[index];

		aiMaterial* material = scene->mMaterials[srcMesh->mMaterialIndex];
		mesh->MaterialName = material->GetName().C_Str();

		UINT startVertex = mesh->Vertices.size(); // 각 Mesh마다 정점이 각각 존재
		for(UINT v = 0; v < srcMesh->mNumVertices; v++)
		{
			Model::ModelVertex vertex;
			memcpy(&vertex.Position, &srcMesh->mVertices[v], sizeof(Vector3));

			if (srcMesh->HasTextureCoords(0))
				memcpy(&vertex.Uv, &srcMesh->mTextureCoords[0][v], sizeof(Vector2));

			if(srcMesh->HasNormals())
				memcpy(&vertex.Normal, &srcMesh->mNormals[v], sizeof(Vector3));

			mesh->Vertices.push_back(vertex);
		}
		for(UINT f = 0; f < srcMesh->mNumFaces; f++)
		{
			aiFace& face = srcMesh->mFaces[f]; // Mesh의 면을 Face라고 함

			for (UINT k = 0; k < face.mNumIndices; k++)
				mesh->Indices.push_back(face.mIndices[k] + startVertex);
		}
		meshes.push_back(mesh);
	}
}

void Converter::ReadSkinData()
{
	for(UINT i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* aiMesh = scene->mMeshes[i];
		if (aiMesh->HasBones() == false) continue;

		asMesh* mesh = meshes[i];

		vector<asBoneWeights> boneWeights;
		boneWeights.assign(mesh->Vertices.size(), asBoneWeights());

		for(UINT b = 0; b < aiMesh->mNumBones; b++)
		{
			aiBone* aiMeshBone = aiMesh->mBones[b];

			/* Bone Index 가져오기 */
			UINT boneIndex = 0;
			for(asBone* bone : bones)
			{
				if(bone->Name == (string)aiMeshBone->mName.C_Str())
				{
					boneIndex = bone->Index;

					break;
				}
			} // for(bone)

			/* Weights 가져오기 */
			for(UINT w = 0; w < aiMeshBone->mNumWeights; w++)
			{
				UINT index = aiMeshBone->mWeights[w].mVertexId; // 영향을 받을 정점 ID
				float weight = aiMeshBone->mWeights[w].mWeight;

				boneWeights[index].AddWeights(boneIndex, weight);
			}
		} // for(b)

		for(UINT w = 0; w < boneWeights.size(); w++)
		{
			boneWeights[i].Normalize();

			asBlendWeight blendWeight;
			boneWeights[w].GetBlendWeights(blendWeight);

			mesh->Vertices[w].BlendIndices = blendWeight.Indices;
			mesh->Vertices[w].BlendWeights = blendWeight.Weights;
		}
	}
}

void Converter::WriteMeshData(wstring savePath)
{
	// Path::CreateFolder() : 해당 지정된 경로만 만들어준다.
	// Path::CreateFolders() : 해당 경로까지 가는데 상위폴더가 없다면 상위폴더까지 만들어준다.

	Path::CreateFolders(Path::GetDirectoryName(savePath));

	BinaryWriter* w = new BinaryWriter();
	w->Open(savePath);

	w->UInt(bones.size());
	for (asBone* bone : bones)
	{
		w->Int(bone->Index);
		w->String(bone->Name);
		w->Int(bone->Parent);
		w->Matrix(bone->Transform);

		SafeDelete(bone);
	}

	w->UInt(meshes.size());
	for (asMesh* meshData : meshes)
	{
		w->String(meshData->Name);
		w->Int(meshData->BoneIndex);

		w->String(meshData->MaterialName);

		w->UInt(meshData->Vertices.size());
		w->Byte(&meshData->Vertices[0], sizeof(Model::ModelVertex) * meshData->Vertices.size());

		w->UInt(meshData->Indices.size());
		w->Byte(&meshData->Indices[0], sizeof(UINT) * meshData->Indices.size());

		SafeDelete(meshData);
	}

	w->Close();
	SafeDelete(w);
}

void Converter::ExportMaterial(wstring savePath, bool bOverwrite)
{
	savePath = L"../../_Textures/" + savePath + L".material";

	/* 덮어씌우지 않는 경우 */
	if(bOverwrite == false)
	{
		if (Path::ExistFile(savePath) == true) // 이미 파일이 존재한다면 수행X
			return;
	}

	ReadMaterialData();
	WriteMaterialData(savePath);
}

void Converter::ReadMaterialData()
{
	for(UINT i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* srcMaterial = scene->mMaterials[i];
		asMaterial* material = new asMaterial();

		material->Name = srcMaterial->GetName().C_Str();

		aiColor3D color; // r,g,b

		// AI_MATKEY_COLOR_AMBIENT : ambient라는 key이름과 0,0 값이 이미 들어가있어서 총 매개변수가 4개
		srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color); // AMBIENT 컬러값이 오른쪽 매개변수인 color에 저장
		material->Ambient = Color(color.r, color.g, color.b, 1.0f);

		srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material->Diffuse = Color(color.r, color.g, color.b, 1.0f);

		srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
		material->Specular = Color(color.r, color.g, color.b, 1.0f);

		srcMaterial->Get(AI_MATKEY_SHININESS, material->Specular.a);

		srcMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color);
		material->Emissive= Color(color.r, color.g, color.b, 1.0f);

		aiString file;

		srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
		material->DiffuseFile = file.C_Str();

		srcMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
		material->SpecularFile = file.C_Str();

		srcMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
		material->NormalFile = file.C_Str();

		materials.push_back(material);
	}
}

void Converter::WriteMaterialData(wstring savePath)
{
	string folder = String::ToString(Path::GetDirectoryName(savePath));
	string file = String::ToString(Path::GetFileName(savePath));

	Path::CreateFolders(folder);

	/* TINYXML을 이용한 xml 코드 */
	Xml::XMLDocument* document = new Xml::XMLDocument();

	Xml::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	Xml::XMLElement* root = document->NewElement("Materials");
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);

	for(asMaterial* material : materials)
	{
		// <Material>을 Xml 파일의 Root 노드의 아래 추가
		Xml::XMLElement* node = document->NewElement("Material");
		root->LinkEndChild(node);

		Xml::XMLElement* element = NULL;

		element = document->NewElement("Name");
		element->SetText(material->Name.c_str());
		//element->SetAttribute("R", 10); // 속성값
		node->LinkEndChild(element);

		element = document->NewElement("DiffuseFile");
		element->SetText(WriteTexture(folder, material->DiffuseFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("SpecularFile");
		element->SetText(WriteTexture(folder, material->SpecularFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("NormalFile");
		element->SetText(WriteTexture(folder, material->NormalFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Ambient");
		element->SetAttribute("R", material->Ambient.r);
		element->SetAttribute("G", material->Ambient.g);
		element->SetAttribute("B", material->Ambient.b);
		element->SetAttribute("A", material->Ambient.a);
		node->LinkEndChild(element);

		element = document->NewElement("Diffuse");
		element->SetAttribute("R", material->Diffuse.r);
		element->SetAttribute("G", material->Diffuse.g);
		element->SetAttribute("B", material->Diffuse.b);
		element->SetAttribute("A", material->Diffuse.a);
		node->LinkEndChild(element);

		element = document->NewElement("Specular");
		element->SetAttribute("R", material->Specular.r);
		element->SetAttribute("G", material->Specular.g);
		element->SetAttribute("B", material->Specular.b);
		element->SetAttribute("A", material->Specular.a);
		node->LinkEndChild(element);

		element = document->NewElement("Emissive");
		element->SetAttribute("R", material->Emissive.r);
		element->SetAttribute("G", material->Emissive.g);
		element->SetAttribute("B", material->Emissive.b);
		element->SetAttribute("A", material->Emissive.a);
		node->LinkEndChild(element);

		SafeDelete(material);
	}

	document->SaveFile((folder + file).c_str());
	SafeDelete(document);
}

string Converter::WriteTexture(string saveFolder, string file)
{
	if (file.length() < 1) return "";

	string fileName = Path::GetFileName(file);

	// 경로명에 '내장 텍스처'가 있다면 텍스처를 return 받고, 없다면 null을 받음
	const aiTexture* texture = scene->GetEmbeddedTexture(file.c_str());

	string path = "";
	if(texture != NULL) // 내장 텍스처가 있는 경우
	{
		path = saveFolder + fileName;

		if(texture->mHeight < 1) // 높이가 없는 경우
		{
			BinaryWriter w;
			w.Open(String::ToWString(path));
			w.Byte(texture->pcData, texture->mWidth);
			w.Close();
		}
		else
		{
			/* Texture 파일 만들기 */
			D3D11_TEXTURE2D_DESC destDesc;
			ZeroMemory(&destDesc, sizeof(D3D11_TEXTURE2D_DESC));
			destDesc.Width = texture->mWidth;
			destDesc.Height = texture->mHeight;
			destDesc.MipLevels = 1;
			destDesc.ArraySize = 1;
			destDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			destDesc.SampleDesc.Count = 1;
			destDesc.SampleDesc.Quality = 0;
			destDesc.Usage = D3D11_USAGE_IMMUTABLE;

			D3D11_SUBRESOURCE_DATA subResource = { 0 };
			subResource.pSysMem = texture->pcData;

			ID3D11Texture2D* dest;

			HRESULT hr;
			hr = D3D::GetDevice()->CreateTexture2D(&destDesc, &subResource, &dest);
			assert(SUCCEEDED(hr));

			D3DX11SaveTextureToFileA(D3D::GetDC(), dest, D3DX11_IFF_PNG, saveFolder.c_str());
		}
	}
	else // 내장 텍스처가 없는 경우
	{
		string directory = Path::GetDirectoryName(String::ToString(this->file));
		string origin = directory + file;
		String::Replace(&origin, "\\", "/");

		if (Path::ExistFile(origin) == false)
			return "";

		path = saveFolder + fileName;
		CopyFileA(origin.c_str(), path.c_str(), FALSE);

		String::Replace(&path, "../../_Textures/", "");
	}

	return Path::GetFileName(path);
}

void Converter::ClipList(vector<wstring>* list)
{
	for (UINT i = 0; i < scene->mNumAnimations; i++)
	{
		aiAnimation* anim = scene->mAnimations[i];

		list->push_back(String::ToWString(anim->mName.C_Str()));			
	}
}

void Converter::ExportAnimClip(UINT index, wstring savePath)
{
	savePath = L"../../_Models/" + savePath + L".clip";

	asClip* clip = ReadClipData(scene->mAnimations[index]);
	WriteClipData(clip, savePath);
}

asClip* Converter::ReadClipData(aiAnimation* animation)
{
	asClip* clip = new asClip();
	clip->Name = animation->mName.C_Str();
	clip->FrameRate = (float)animation->mTicksPerSecond;
	clip->FrameCount = (UINT)animation->mDuration + 1; // Frame 길이

	vector<asClipNode> aniNodeInfos;
	for (UINT i = 0; i < animation->mNumChannels; i++)
	{
		aiNodeAnim* aniNode = animation->mChannels[i];

		asClipNode aniNodeInfo;
		aniNodeInfo.Name = aniNode->mNodeName;

		// 가장 큰 프레임 키값 찾기
		UINT keyCount = max(aniNode->mNumPositionKeys, aniNode->mNumScalingKeys);
		keyCount = max(keyCount, aniNode->mNumRotationKeys);

		for (UINT k = 0; k < keyCount; k++)
		{
			asKeyframeData frameData;

			bool bFound = false;
			UINT t = aniNodeInfo.Keyframe.size();

			// D3DX_16F_EPSILON : 2바이트 float에 대한 오차값, (Equal이 아니라 Nearly Equal이다)
			// 애니메이션의 키값 근처의 프레임을 찾기
			if(fabsf((float)aniNode->mPositionKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON)
			{
				// 키 찾음
				aiVectorKey key = aniNode->mPositionKeys[k];
				frameData.Time = (float)key.mTime;
				memcpy_s(&frameData.Translation, sizeof(Vector3), &key.mValue, sizeof(aiVector3D));

				bFound = true;
			}

			// 쿼터니온이 뭔지 찾아놓기, 사원수가 뭔지 찾아놓기
			if (fabsf((float)aniNode->mRotationKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON)
			{
				aiQuatKey key = aniNode->mRotationKeys[k];
				frameData.Time = (float)key.mTime;

				frameData.Rotation.x = key.mValue.x; // x,y,z,w 순서가 달라서 memcopy 안하고 복사함
				frameData.Rotation.y = key.mValue.y;
				frameData.Rotation.z = key.mValue.z;
				frameData.Rotation.w = key.mValue.w;

				bFound = true;
			}

			if (fabsf((float)aniNode->mScalingKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON)
			{
				aiVectorKey key = aniNode->mScalingKeys[k];
				frameData.Time = (float)key.mTime;
				memcpy_s(&frameData.Scale, sizeof(Vector3), &key.mValue, sizeof(aiVector3D));

				bFound = true;
			}

			if (bFound == true)
				aniNodeInfo.Keyframe.push_back(frameData);
		}//for(k)

		// 내가 저장한 Keyframe 사이즈가 실제 애니메이션의 frame 개수보다 적다면
		// 나머지 부분을 애니메이션의 마지막 키프레임으로 전부 채운다.
		if (aniNodeInfo.Keyframe.size() < clip->FrameCount)
		{
			UINT count = clip->FrameCount - aniNodeInfo.Keyframe.size();

			asKeyframeData keyFrame = aniNodeInfo.Keyframe.back();

			for (UINT n = 0; n < count; n++)
				aniNodeInfo.Keyframe.push_back(keyFrame);
		}
		clip->Duration = max(clip->Duration, aniNodeInfo.Keyframe.back().Time);

		aniNodeInfos.push_back(aniNodeInfo);
	}

	ReadKeyframeData(clip, scene->mRootNode, aniNodeInfos);

	return clip;
}

void Converter::ReadKeyframeData(asClip* clip, aiNode* node, vector<asClipNode>& aiNodeInfos)
{
	asKeyframe* keyframe = new asKeyframe();
	keyframe->BoneName = node->mName.C_Str();

	// 각 본마다 프레임별로 for문을 돌면서 해당 Bone이름과 매칭되는 Bone이 있는지 찾은 후
	// 일치하는 Name이 있다면 존재하는 FrameData를 그대로 사용하고, 없다면 원래 Bone이 가지고있는 Transform을 넣어줌
	for(UINT i = 0; i < clip->FrameCount; i++)
	{
		asClipNode* asClipNode = NULL;

		for(UINT n = 0; n < aiNodeInfos.size(); n++)
		{
			if(aiNodeInfos[n].Name == node->mName)
			{
				asClipNode = &aiNodeInfos[n];

				break;
			}
		}//for(n)

		asKeyframeData frameData;
		if(asClipNode == NULL)
		{
			Matrix transform(node->mTransformation[0]);
			D3DXMatrixTranspose(&transform, &transform);

			frameData.Time = (float)i;
			D3DXMatrixDecompose(&frameData.Scale, &frameData.Rotation, &frameData.Translation, &transform);
		}
		else
		{
			frameData = asClipNode->Keyframe[i];
		}
		keyframe->Transforms.push_back(frameData);
	}
	clip->Keyframes.push_back(keyframe);

	for (UINT i = 0; i < node->mNumChildren; i++)
		ReadKeyframeData(clip, node->mChildren[i], aiNodeInfos);
}

void Converter::WriteClipData(asClip * clip, wstring savePath)
{
	Path::CreateFolders(Path::GetDirectoryName(savePath));

	BinaryWriter* w = new BinaryWriter();
	w->Open(savePath);

	w->String(clip->Name);
	w->Float(clip->Duration);
	w->Float(clip->FrameRate);
	w->UInt(clip->FrameCount);

	w->UInt(clip->Keyframes.size());
	for (asKeyframe* keyframe : clip->Keyframes)
	{
		w->String(keyframe->BoneName);

		w->UInt(keyframe->Transforms.size());
		w->Byte(&keyframe->Transforms[0], sizeof(asKeyframeData) * keyframe->Transforms.size());

		SafeDelete(keyframe);
	}

	w->Close();
	SafeDelete(w);
}

