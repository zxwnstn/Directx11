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

	class ObjModelBuilder
	{
	public:
		ObjModelBuilder(Model3D* myModel);

		FinalModelBuilder SetObject(const std::string& objectName);

	private:
		Model3D* myModel;
	};

	class CustomModelBuilder
	{
	public:
		CustomModelBuilder(Model3D* myModel);

		CustomModelBuilder& SetMesh(const std::string& meshName);
		CustomModelBuilder& SetMaterial(const std::string& materialName);

		FinalModelBuilder Finish();

	private:
		Model3D* myModel;
	};

	class ModelBuilder
	{
	public:
		ModelBuilder(Model3D* myModel);

		FbxModelBuilder buildFromFBX();
		ObjModelBuilder buildFromOBJ();
		CustomModelBuilder buildCustum();

	private:
		Model3D* myModel;
	};

}
