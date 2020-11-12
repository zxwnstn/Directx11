#pragma once

#include "Core/App.h"

#include "Common/Camera.h"
#include "Common/Transform.h"

class SandBox : public Layer
{
public:
	void OnUpdate(float dt) override;
	void OnAttach() override;
	void OnDettach() override;

	std::shared_ptr<class ModelBuffer> buffer;
	std::shared_ptr<class ModelBuffer> buffer2;

	Transform transform;

	std::shared_ptr<class ModelBuffer> TextureBuffer;
	std::shared_ptr<class Texture> texture;

	std::shared_ptr<Camera> ortho;
	std::shared_ptr<Camera> perspective;
};