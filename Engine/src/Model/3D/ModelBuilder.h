#pragma once

namespace Engine {

	class Model3D;

	//Has no any attribute controller
	class FinalModelBuilder
	{
	public:
		FinalModelBuilder(Model3D* myModel);
		inline operator std::shared_ptr<Model3D>() const { return std::shared_ptr<Model3D>(myModel); }

	private:
		Model3D* myModel;
	};

	class FbxModelBuilder
	{
	public:
		FbxModelBuilder(Model3D* myModel);

		FinalModelBuilder SetSkeleton(const std::string& skeletonName);

	private:
		Model3D* myModel;
	};

	class NoneFbxModelBuilder
	{
	public:
		NoneFbxModelBuilder(Model3D* myModel);

		NoneFbxModelBuilder& SetIndices();
		NoneFbxModelBuilder& SetVertices();
		NoneFbxModelBuilder& SetTexture();
		NoneFbxModelBuilder& SetMaterial();

		FinalModelBuilder Finish();

	private:
		Model3D* myModel;
	};

	class ModelBuilder
	{
	public:
		ModelBuilder(Model3D* myModel);

		FbxModelBuilder buildFromFBX();
		NoneFbxModelBuilder buildCustum();

	private:
		Model3D* myModel;
	};

}
