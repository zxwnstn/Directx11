#include "pch.h"

#include "Util/Math.h"
#include "SkeletalAnimation.h"
#include "Skeleton.h"
#include "File/FbxLoader.h"

namespace Engine {

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

		return { &KeyFrames[i - 1], &KeyFrames[i] };
	}

	std::vector<KeyFramePair> SkeletalAnimtion::GetKeyFrames(float elapsedTime)
	{

		std::vector<KeyFramePair> ret;
		for (auto& jointAnimation : JointAnimations)
		{
			if (jointAnimation.KeyFrames.empty()) 
				continue;

			ret.push_back(jointAnimation.GetKeyFramePair(elapsedTime));
		}

		return ret;
	}


	std::unordered_map<std::string, std::shared_ptr<SkeletalAnimtion>> SkeletalAnimationArchive::s_Animations;
	std::unordered_map<std::string, std::vector<std::string>> SkeletalAnimationArchive::s_AnimList;

	bool SkeletalAnimationArchive::Add(const std::string & skeletonName, const std::string & animName)
	{
		std::string fullName = skeletonName + "/" + animName;
		if (Has(fullName)) return false;

		std::shared_ptr<SkeletalAnimtion> animation(new SkeletalAnimtion);
		s_Animations[fullName] = animation;
		s_AnimList[skeletonName].push_back(animName);

		return true;
	}

	void SkeletalAnimationArchive::Delete(const std::string & skeletonName, const std::string & animName)
	{
		std::string fullName = skeletonName + "/" + animName;
		if (!Has(fullName)) return;

		auto animation = s_Animations.find(fullName);
		s_Animations.erase(animation);

		auto listItem = std::find(s_AnimList[skeletonName].begin(), s_AnimList[skeletonName].end(), animName);
		s_AnimList[skeletonName].erase(listItem);
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

	void SkeletalAnimationArchive::Shutdown()
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

	std::shared_ptr<SkeletalAnimtion> SkeletalAnimationArchive::GetAnimation(const std::string & skeletonName, const std::string & animName)
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

		if (s_Animations[fullName]->JointAnimations.empty())
		{
			std::cout << "The animation is empty";
			return 0.0f;
		}
		
		return s_Animations[fullName]->JointAnimations.begin()->Duration;
	}

	float SkeletalAnimationArchive::GetKeyInterval(const std::string & skeletonName, const std::string & animName)
	{
		std::string fullName = skeletonName + "/" + animName;
		if (!Has(fullName))
			return 0.0f;

		if(s_Animations[fullName]->JointAnimations.empty())
		{
			std::cout << "The animation is empty";
			return 0.0f;
		}

		float interval = s_Animations[fullName]->JointAnimations[0].KeyFrames[1].Start -
			s_Animations[fullName]->JointAnimations[0].KeyFrames[0].Start;

		return interval;
	}

	void SkeletalAnimationPlayer::Play(const std::string& skeletonName, std::shared_ptr<AnimationInform> inform)
	{
		using namespace DirectX;

		std::vector<KeyFramePair> keyFrames = SkeletalAnimationArchive::s_Animations[skeletonName + "/" + inform->CurAnim]->GetKeyFrames(inform->Elapsedtime);
		int i = 0;
		for (auto&[first, second]: keyFrames)
		{
			if (!second)
			{
				inform->MySkinnedTransforms[i] = Util::GetTransform(first->Translation, first->RotationQuat, first->Scale, false);
			}
			else
			{
				float ratio = (inform->Elapsedtime - first->Start) / inform->KeyInterval;
				inform->MySkinnedTransforms[i] =  Util::GetLerpTransform(
					first->Translation, second->Translation, 
					first->RotationQuat, second->RotationQuat, 
					first->Scale, second->Scale, 
					ratio
				);
			}
			i++;
		}

		auto& joints = SkeletonArchive::Get(skeletonName)->Joints;
		for (size_t i = 0; i < joints.size(); ++i)
		{
			Util::CalcFinalSkinnedTransform(joints[i].Offset, inform->MySkinnedTransforms[i], 0.01f);
		}
	}
}
