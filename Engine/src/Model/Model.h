#pragma once

#include "3D/ModelBuilder.h"
#include "2D/2DModelBuilder.h"
#include "Common/Transform.h"
#include "2D/SpriteAnimation.h"

namespace Engine {

	class Model2D
	{
	private:
		Model2D() = default;

	public:
		static ModelBuilder2D Create();
		
		void Update(float dt);

	public:
		Transform m_Transform;

	private:
		std::shared_ptr<class Texture> m_Texture;
		std::shared_ptr<class SpriteAnimation> m_Animation;

		friend class ModelBuilder2D;
		friend class Renderer;
	};

	class Model3D
	{
	private:
		Model3D() = default;

	public:
		static ModelBuilder Create();
		
		void Update(float dt);
		bool SetAnimation(const std::string& animationName, bool loop);

	private:
		void animationUpdate(float dt);

	public:
		//Can't be Shared
		std::shared_ptr<struct AnimationInform> m_Animation;
		std::shared_ptr<struct MaterialSet> m_MaterialSet;
		Transform m_Transform;

	private:
		//Can be Shared
		std::shared_ptr<struct Skeleton> m_Skeleton;
		std::shared_ptr<class ModelBuffer> m_ModelBuffer;

		friend class FbxModelBuilder;
		friend class ObjModelBuilder;
		friend class CustomModelBuilder;
		friend class Renderer;
	};

}
