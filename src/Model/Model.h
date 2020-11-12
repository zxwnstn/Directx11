#pragma once


class Model2D
{



};

class Model3D
{
public:
	void Bind();
	void Update(float dt);

private:
	//Can be Shared
	std::shared_ptr<class Skeleton> m_Skeleton;
	std::shared_ptr<class Material> m_Material;
	std::shared_ptr<class ModelBuffer> m_ModelBuffer;
	std::shared_ptr<class Texture> m_Texture;

	//intermediate
	std::shared_ptr<class AnimationInform> m_Animation;

	//Can't be Shared
	std::shared_ptr<class Transform> m_Transform;
};