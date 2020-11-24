#include "pch.h"

#include "2DModelBuilder.h"

#include "Model/Model.h"
#include "Renderer/Texture.h"

namespace Engine {

	ModelBuilder2D::ModelBuilder2D(Model2D * myModel)
		: myModel(myModel)
	{
	}

	ModelBuilder2D & ModelBuilder2D::SetTexture(const std::string & texture)
	{
		myModel->m_Texture = TextureArchive::Get(texture);
		return *this;
	}

	ModelBuilder2D & ModelBuilder2D::SetAnimation(const std::string & animation)
	{
		return *this;
	}

}
