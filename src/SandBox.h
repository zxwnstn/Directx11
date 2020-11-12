#pragma once

#include "Core/App.h"

#include "Resource/Camera.h"
#include "Resource/Animation.h"
#include "Resource/Transform.h"

class SandBox : public Layer
{
public:
	void OnUpdate(float dt) override;
	void OnAttach() override;
	void OnDettach() override;

	std::shared_ptr<class Buffer> buffer;
	std::shared_ptr<class Buffer> buffer2;

	Transform transform;

	std::shared_ptr<class Buffer> TextureBuffer;
	std::shared_ptr<class Texture> texture;

	std::shared_ptr<Camera> ortho;
	std::shared_ptr<Camera> perspective;

	Skeleton skeleton;
};