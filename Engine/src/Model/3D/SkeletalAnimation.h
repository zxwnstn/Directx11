#pragma once

namespace Engine {

	struct AnimationInform
	{
		std::vector<std::string> AnimList;
		float Elapsedtime = 0.0f;
		float Duration = 0.0f;
		float KeyInterval = 0.0f;
		float Accelation = 1.0f;
		std::string CurAnim;
		std::string TranslatedAnim;
		bool AnimTranslated = false;
		bool Expired = false;
		bool Loop = true;
		bool Reverse = false;

		mat4 MySkinnedTransforms[100];
	};

	struct KeyFrame
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & Start;
			ar & Translation;
			ar & RotationQuat;
			ar & Scale;
		}

		float Start;
		vec3 Translation;
		vec4 RotationQuat;
		vec3 Scale;

		bool operator==(const KeyFrame& rhs)
		{
			return Translation.x == rhs.Translation.x &&
				Translation.y == rhs.Translation.y &&
				Translation.z == rhs.Translation.z;
		}
	};

	using KeyFramePair = std::pair<KeyFrame*, KeyFrame*>;

	struct JointAnimation
	{
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & Duration;
			ar & KeyFrames;
		}
		float Duration = 0.0f;
		std::vector<KeyFrame> KeyFrames;

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
		static bool Add(const std::string& skeletonName, const std::string& animName);
		static void Delete(const std::string& skeletonName, const std::string& animName);
		static std::vector<std::string>& GetAnimList(const std::string& skeletonName);
		static std::shared_ptr<SkeletalAnimtion> GetAnimation(const std::string& skeletonName, const std::string& animName);
		static float GetAnimationDuration(const std::string& skeletonName, const std::string& animName);
		static float GetKeyInterval(const std::string& skeletonName, const std::string& animName);
		static bool Has(const std::string& skeletonName, const std::string& animName);
		static bool Has(const std::string& fullName);
		static void Shutdown();

	private:
		static std::vector<KeyFramePair> GetAnimationKeys(const std::string& skeltonName, const std::string& animName, float elapsedTime);
		static std::vector<KeyFramePair> GetAnimationKeys(const std::string& fullName, float elapsedTime);

		//Save as full name which combine SkeletonName + AnimationName
		//Eg. i fskeleton name is 'human' and animation name is 'idle' then animtion name is human/idle
		static std::unordered_map<std::string, std::shared_ptr<SkeletalAnimtion>> s_Animations;

		//Save names sperated which SkeletonName and vector AnimationNames
		//Eg. if skeleton name is 'human' and animation name is 'idle' then key human and push idle as a value vector
		static std::unordered_map<std::string, std::vector<std::string>> s_AnimList;

		friend class SkeletalAnimationPlayer;
	};

	class SkeletalAnimationPlayer
	{
	public:
		static void Play(const std::string& skeletonName, std::shared_ptr<AnimationInform> inform);
	};

}
