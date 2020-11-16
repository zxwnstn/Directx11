#pragma once

class RenderingThread : public QThread
{
public:
	RenderingThread();

	void run() override;
	void UpdateControl();

private:
	bool resized = false;
	bool myFlag = true;
	float speed = 0.016f;

	uint32_t w;
	uint32_t h;

	std::shared_ptr<Engine::Model3D> model;
	std::shared_ptr<Engine::Camera> perspective;
	Engine::Light light;

	friend class Viewport;
};