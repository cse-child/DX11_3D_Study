#pragma once

struct ModelKeyframeData
{
	float Time;

	Vector3 Scale;
	Quaternion Rotation;
	Vector3 Translation;
};

struct ModelKeyframe
{
	wstring BoneName;
	vector<ModelKeyframeData> Transforms;
};

class ModelClip
{
public:
	friend class Model;

private:
	ModelClip();
	~ModelClip();

public:
	float Duration() { return duration; }
	float FrameRate() { return frameRate; }
	UINT FrameCount() { return frameCount; }

	ModelKeyframe* Keyframe(wstring name);

private:
	wstring name;

	float duration;
	float frameRate;
	UINT frameCount;

	// unordered_map은 map과 다르게 해시코드를 사용하여 많은 데이터를 찾아올때 효율이 좋고 빠르다.
	// unordered_map과 map의 차이점 찾아서 정리하기
	unordered_map<wstring, ModelKeyframe *> keyframeMap;
};