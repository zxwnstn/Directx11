#pragma once

namespace Engine {

	class Model2D;

	class ModelBuilder2D
	{
	public:
		ModelBuilder2D(Model2D* myModel);

		ModelBuilder2D& SetTexture(const std::string& texture);
		ModelBuilder2D& SetAnimation(const std::string& animation);

		inline operator std::shared_ptr<Model2D>() const { return std::shared_ptr<Model2D>(myModel); }

	private:
		Model2D* myModel;
	};

}
