#include "pch.h"

#include "SkeletalAnimation.h"
#include "Skeleton.h"
#include "File/FbxLoader.h"

KeyFramePair JointAnimation::GetKeyFramePair(float elapsedTime)
{
	uint32_t timePoint = uint32_t((elapsedTime / Duration) * float(KeyFrames.size()));
	if(timePoint == 0)
		return { &KeyFrames[timePoint], nullptr };

	if (timePoint > KeyFrames.size())
		return { &KeyFrames[timePoint - 1], nullptr };

	return { &KeyFrames[timePoint - 1], &KeyFrames[timePoint] };
}

std::vector<KeyFramePair> SkeletalAnimtion::GetKeyFrames(float elapsedTime)
{
	std::vector<KeyFramePair> ret;
	for (auto jointAnimation : JointAnimations)
		ret.emplace_back(jointAnimation.GetKeyFramePair(elapsedTime));

	return ret;
}


std::unordered_map<std::string, SkeletalAnimtion*> SkeletalAnimationArchive::s_Animations;

void SkeletalAnimationArchive::Add(const std::string & skeltonName, const std::string & animName, SkeletalAnimtion* animation)
{
	std::string fullName = skeltonName + "/" + animName;
	if (Has(fullName)) return;

	s_Animations[fullName] = animation;
}

bool SkeletalAnimationArchive::Has(const std::string & skeltonName, const std::string & animName)
{
	std::string fullName = skeltonName + "/" + animName;
	return Has(fullName);
}

bool SkeletalAnimationArchive::Has(const std::string & fullName)
{
	auto find = s_Animations.find(fullName);
	return find != s_Animations.end();
}

std::vector<KeyFramePair> SkeletalAnimationArchive::GetAnimationKeys(const std::string & skeltonName, const std::string & animName, float elapsedTime)
{
	std::string fullName = skeltonName + "/" + animName;
	return GetAnimationKeys(fullName, elapsedTime);
}

std::vector<KeyFramePair> SkeletalAnimationArchive::GetAnimationKeys(const std::string & fullName, float elapsedTime)
{
	if (!Has(fullName))
	{
		std::cout << "The Animation doen't exist!\n";
		return std::vector<KeyFramePair>();
	}
	return s_Animations[fullName]->GetKeyFrames(elapsedTime);
}

void SkeletalAnimationPlayer::Play(std::shared_ptr<AnimationInform> inform)
{
	using namespace DirectX;

	std::vector<KeyFramePair> keyFrames = SkeletalAnimationArchive::s_Animations[inform->CurAnim]->GetKeyFrames(inform->Elapsedtime);

	int i = 0;
	for (auto& keyFrame : keyFrames)
	{
		if (!keyFrame.second)
		{
			XMVECTOR S = XMLoadFloat3(&keyFrame.first->Scale);
			XMVECTOR P = XMLoadFloat3(&keyFrame.first->Translation);
			XMVECTOR Q = XMLoadFloat4(&keyFrame.first->RotationQuat);
			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMStoreFloat4x4(&inform->MySkinnedTransforms[i], XMMatrixAffineTransformation(S, zero, Q, P));
		}
		else
		{
			float lerpPercent = (inform->Elapsedtime - keyFrame.first->Start) / (keyFrame.second->Start - keyFrame.first->Start);

			XMVECTOR s0 = XMLoadFloat3(&keyFrame.first->Scale);
			XMVECTOR s1 = XMLoadFloat3(&keyFrame.second->Scale);

			XMVECTOR p0 = XMLoadFloat3(&keyFrame.first->Translation);
			XMVECTOR p1 = XMLoadFloat3(&keyFrame.second->Translation);

			XMVECTOR q0 = XMLoadFloat4(&keyFrame.first->RotationQuat);
			XMVECTOR q1 = XMLoadFloat4(&keyFrame.second->RotationQuat);

			XMVECTOR S = XMVectorLerp(s0, s1, lerpPercent);
			XMVECTOR P = XMVectorLerp(p0, p1, lerpPercent);
			XMVECTOR Q = XMQuaternionSlerp(q0, q1, lerpPercent);

			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			XMStoreFloat4x4(&inform->MySkinnedTransforms[i], XMMatrixAffineTransformation(S, zero, Q, P));
		}
		i++;
	}
}
