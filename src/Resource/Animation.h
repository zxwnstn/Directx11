#pragma once

struct KeyFrame
{
	float start;
	float end;

	DirectX::XMFLOAT3 Translation;
	DirectX::XMFLOAT4 RotationQuat;
	DirectX::XMFLOAT3 Scale;
};

//조인트 하나당 Animation
struct AnimationClip
{
	float Start;
	float End;
	float cur;
	float Duration;

	std::vector<KeyFrame> KeyFrames;
	DirectX::XMFLOAT4X4 InterpoledMat;

	void Interpolate(float dt);
};

struct Joint
{
	std::string Name;
	int Parent;
	DirectX::XMFLOAT4X4 OffsetMat;
	std::unordered_map<std::string, AnimationClip> Anim;

	void Update(float dt, std::vector<DirectX::XMFLOAT4X4>& finalMat);

};

struct Skeleton
{
	std::vector<Joint> Joints;
	std::vector<DirectX::XMFLOAT4X4> FinalMat;
	void Update(float dt);
};