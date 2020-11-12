#include "pch.h"

#include "ModelBuilder.h"
#include "Model/Model.h"

/***************************************/
/********  finalModel Builder  *********/
/***************************************/

FinalModelBuilder::FinalModelBuilder(Model3D * myModel)
	: myModel(myModel)
{
}



/***************************************/
/********  FbxModel Builder  ***********/
/***************************************/
FbxModelBuilder::FbxModelBuilder(Model3D * myModel)
	: myModel(myModel)
{}

FinalModelBuilder FbxModelBuilder::SetSkeleton(const std::string & skeletonName)
{
	return FinalModelBuilder(myModel);
}



/***************************************/
/*******  NoneFbxModel Builder  ********/
/***************************************/
NoneFbxModelBuilder::NoneFbxModelBuilder(Model3D * myModel)
	: myModel(myModel)
{}

NoneFbxModelBuilder & NoneFbxModelBuilder::SetIndices()
{
	return *this;
}

NoneFbxModelBuilder & NoneFbxModelBuilder::SetVertices()
{
	return *this;
}

NoneFbxModelBuilder & NoneFbxModelBuilder::SetTexture()
{
	return *this;
}

NoneFbxModelBuilder & NoneFbxModelBuilder::SetMaterial()
{
	return *this;
}

FinalModelBuilder NoneFbxModelBuilder::Finish()
{
	return FinalModelBuilder(myModel);
}

/***************************************/
/*********  Model Builder  *************/
/***************************************/
ModelBuilder::ModelBuilder(Model3D * myModel)
	: myModel(myModel)
{}

FbxModelBuilder ModelBuilder::buildFromFBX()
{
	return FbxModelBuilder(myModel);
}

NoneFbxModelBuilder ModelBuilder::buildCustum()
{
	return NoneFbxModelBuilder(myModel);
}

