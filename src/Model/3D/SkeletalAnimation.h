#pragma once

#include "Skeleton.h"
#include "File/FbxLoader.h"

struct AnimationInform
{
	//this key uint is will be enum
	std::unordered_map<uint32_t, std::string> Animations;
	float Elapsedtime;
	std::string CurAnim;
	std::string TranslatedAnim;
	bool AnimTranslated;

	std::vector<DirectX::XMFLOAT4X4> MySkinnedTransforms;
};

struct KeyFrame
{
	float Start;
	DirectX::XMFLOAT3 Translation;
	DirectX::XMFLOAT4 RotationQuat;
	DirectX::XMFLOAT3 Scale;
};

using KeyFramePair = std::pair<KeyFrame*, KeyFrame*>;

struct JointAnimation
{
	float Duration;
	std::unordered_map<uint32_t ,KeyFrame> KeyFrames;

	KeyFramePair GetKeyFramePair(float elapsedTime);
};

struct SkeletalAnimtion
{
	std::vector<JointAnimation> JointAnimations;

	std::vector<KeyFramePair> GetKeyFrames(float elapsedTime);
};

class SkeletalAnimationArchive
{
public:
	static void Add(const std::string& skeltonName, const std::string& animName);
	static bool Has(const std::string& skeltonName, const std::string& animName);
	static bool Has(const std::string& fullName);

private:
	static std::vector<KeyFramePair> GetAnimationKeys(const std::string& skeltonName, const std::string& animName, float elapsedTime);
	static std::vector<KeyFramePair> GetAnimationKeys(const std::string& fullName, float elapsedTime);

	//Save as full name which combine SkeletonName + AnimationName
	//Eg. skeleton name is 'human' and animation name is 'idle' then animtion name is human/idle
	static std::unordered_map<std::string, SkeletalAnimtion> s_Animations;

	friend class SkeletalAnimationPlayer;
};

class SkeletalAnimationPlayer
{
public:
	static void Play(std::shared_ptr<AnimationInform> inform);
};