#pragma once

#include "Renderer/Renderer.h"
#include "3D/ModelBuilder.h"
#include "Common/Transform.h"

class Model2D
{



};

class Model3D
{
private:
	Model3D(const std::string& ShaderName);

public:
	void Update(float dt);
	bool SetAnimation(const std::string& animationName, bool loop);

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
	std::shared_ptr<class Material> m_Material;
	std::shared_ptr<class ModelBuffer> m_ModelBuffer;
	std::vector<std::shared_ptr<class Texture>> m_Textures;
	//std::shared_ptr<class Controller> m_Controler;

	std::string m_Shader;

	friend class NoneFbxModelBuilder;
	friend class FbxModelBuilder;
	friend class Renderer;
};

