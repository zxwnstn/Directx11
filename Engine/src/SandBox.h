#pragma once

#include "Core/App.h"

#include "Common/Camera.h"

class SandBox : public Layer
{
public:
	void OnUpdate(float dt) override;
	void OnAttach() override;
	void OnDettach() override;

private:
	void controlUpdate(float dt);

	std::shared_ptr<class ModelBuffer> buffer;
	std::shared_ptr<class Texture> texture;

	std::shared_ptr<class Model3D> model;

	std::shared_ptr<Camera> ortho;
	std::shared_ptr<Camera> perspective;
};