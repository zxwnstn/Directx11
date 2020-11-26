#pragma once

#include "Renderer/Renderer.h"
#include "3D/ModelBuilder.h"
#include "2D/2DModelBuilder.h"
#include "Common/Transform.h"
#include "2D/SpriteAnimation.h"

namespace Engine {

	class Model2D
	{
	public:
		Model2D(const std::string& shaderName);

	public:
		static ModelBuilder2D Create(RenderingShader type, std::string&& shaderName = "");

	public:
		void Update(float dt);
		inline void ActivateShadow(bool activate) { m_ActiveShadow = activate; }
		inline bool isActivateShadow() { return m_ActiveShadow; }

	public:
		Transform m_Transform;

	private:
		std::shared_ptr<class Texture> m_Texture;
		std::shared_ptr<class SpriteAnimation> m_Animation;

		std::string m_Shader;
		bool m_ActiveShadow = true;

		friend class ModelBuilder2D;
		friend class Renderer;
	};

	class Model3D
	{
	public:
		enum class Type
		{
			Static,
			Skeletal,
			None
		};
	private:
		Model3D(const std::string& ShaderName);

	public:
		void Update(float dt);
		bool SetAnimation(const std::string& animationName, bool loop);
		void SetShader(const std::string& shader);
		inline void ActivateShadow(bool activate) { m_ActiveShadow = activate; }
		inline bool isActivateShadow() { return m_ActiveShadow; }
		std::shared_ptr<struct MaterialSet> m_MaterialSet;

	public:
		static ModelBuilder Create(RenderingShader type, std::string&& ShaderName = "");

	private:
		void animationUpdate(float dt);

	public:
		//Can't be Shared
		std::shared_ptr<struct AnimationInform> m_Animation;
		Transform m_Transform;

	private:
		//Can be Shared
		std::shared_ptr<struct Skeleton> m_Skeleton;
		std::shared_ptr<class ModelBuffer> m_ModelBuffer;
		std::shared_ptr<class Texture> m_Texture;
		//std::shared_ptr<class Controller> m_Controler;

		std::string m_Shader;
		Type m_Type;
		bool m_ActiveShadow = true;

		friend class NoneFbxModelBuilder;
		friend class FbxModelBuilder;
		friend class Renderer;
		friend class StaticModelBuilder;
	};

}
