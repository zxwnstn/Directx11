#include "pch.h"

#include "SkeletalAnimation.h"
#include "Skeleton.h"
#include "File/FbxLoader.h"

KeyFramePair JointAnimation::GetKeyFramePair(float elapsedTime)
{
	size_t i;
	for (i = 0; i < KeyFrames.size(); ++i)
	{
		if (KeyFrames[i].Start >= elapsedTime)
			break;
	}
	if (i == KeyFrames.size())
	{
		return { &KeyFrames[i - 1], nullptr };
	}

	if (i == 0)
		return { &KeyFrames[0], nullptr };

	return {&KeyFrames[i - 1], &KeyFrames[i]};
}

std::vector<KeyFramePair> SkeletalAnimtion::GetKeyFrames(float elapsedTime)
{

	std::vector<KeyFramePair> ret;
	for (auto& jointAnimation : JointAnimations)
		ret.push_back(jointAnimation.GetKeyFramePair(elapsedTime));

	return ret;
}


std::unordered_map<std::string, SkeletalAnimtion*> SkeletalAnimationArchive::s_Animations;
std::unordered_map<std::string, std::vector<std::string>> SkeletalAnimationArchive::s_AnimList;

bool SkeletalAnimationArchive::Add(const std::string & skeletonName, const std::string & animName)
{
	std::string fullName = skeletonName + "/" + animName;
	if (Has(fullName)) return false;

	SkeletalAnimtion* animation = new SkeletalAnimtion;
	s_Animations[fullName] = animation;
	s_AnimList[skeletonName].push_back(animName);

	return true;
}

bool SkeletalAnimationArchive::Has(const std::string & skeletonName, const std::string & animName)
{
	std::string fullName = skeletonName + "/" + animName;
	return Has(fullName);
}

bool SkeletalAnimationArchive::Has(const std::string & fullName)
{
	auto find = s_Animations.find(fullName);
	return find != s_Animations.end();
}

void SkeletalAnimationArchive::Shudown()
{
	s_Animations.clear();
}

std::vector<KeyFramePair> SkeletalAnimationArchive::GetAnimationKeys(const std::string & skeletonName, const std::string & animName, float elapsedTime)
{
	std::string fullName = skeletonName + "/" + animName;
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

std::vector<std::string>& SkeletalAnimationArchive::GetAnimList(const std::string & skeletonName)
{
	auto find = s_AnimList.find(skeletonName);
	if (find == s_AnimList.end())
	{
		std::cout << "The AnimationList doen't exist!\n";
		//assert(flase);
		return  s_AnimList[skeletonName];
	}
	return s_AnimList[skeletonName];
}

SkeletalAnimtion * SkeletalAnimationArchive::GetAnimation(const std::string & skeletonName, const std::string & animName)
{
	std::string fullName = skeletonName + "/" + animName;
	if (!Has(fullName))
		return nullptr;
	return s_Animations[fullName];
}

float SkeletalAnimationArchive::GetAnimationDuration(const std::string & skeletonName, const std::string & animName)
{
	std::string fullName = skeletonName + "/" + animName;
	if (!Has(fullName))
		return 0.0f;

	return s_Animations[fullName]->JointAnimations.begin()->Duration;
}

float SkeletalAnimationArchive::GetKeyInterval(const std::string & skeletonName, const std::string & animName)
{
	std::string fullName = skeletonName + "/" + animName;
	if (!Has(fullName))
		return 0.0f;

	float interval = s_Animations[fullName]->JointAnimations[0].KeyFrames[1].Start -
	s_Animations[fullName]->JointAnimations[0].KeyFrames[0].Start;

	return interval;
}

void SkeletalAnimationPlayer::Play(const std::string& skeletonName, std::shared_ptr<AnimationInform> inform)
{
	using namespace DirectX;

	std::vector<KeyFramePair> keyFrames = SkeletalAnimationArchive::s_Animations[skeletonName + "/" + inform->CurAnim]->GetKeyFrames(inform->Elapsedtime);
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
			float lerpPercent = (inform->Elapsedtime - keyFrame.first->Start) / inform->KeyInterval;

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

	auto& joints = SkeletonArchive::Get(skeletonName)->Joints;
	for (size_t i = 0; i < joints.size(); ++i)
	{
		XMMATRIX skinnedTransform = XMLoadFloat4x4(&inform->MySkinnedTransforms[i]);
		skinnedTransform = XMMatrixMultiply(joints[i].Offset, skinnedTransform);
		skinnedTransform *= XMMatrixScaling(0.01f, 0.01f, 0.01f);
		XMStoreFloat4x4(&inform->MySkinnedTransforms[i], XMMatrixTranspose(skinnedTransform));
	}

}
