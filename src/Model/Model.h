#pragma once

#include "Renderer/Renderer.h"

class Model2D
{



};

class Model3D
{
private:
	Model3D(const std::string& ShaderName);

public:
	void Update(float dt);
	/*
		setters
	*/

public:
	static class ModelBuilder Create(RenderingShader type, std::string&& ShaderName = "");

private:
	void Bind();

private:
	//Can be Shared
	std::shared_ptr<class Skeleton> m_Skeleton;
	std::shared_ptr<class Material> m_Material;
	std::shared_ptr<class ModelBuffer> m_ModelBuffer;
	std::shared_ptr<class Texture> m_Texture;
	//std::shared_ptr<class Controller> m_Controler;

	//Can't be Shared
	std::shared_ptr<struct AnimationInform> m_Animation;
	std::shared_ptr<class Transform> m_Transform;
	std::shared_ptr<class Colider> m_Colider;

	std::string m_Shader;

	friend class NoneFbxModelBuilder;
	friend class FbxModelBuilder;
	friend class Renderer;
};

