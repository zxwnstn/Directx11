#pragma once

class SandBox 
{
public:
	void OnUpdate(float dt);
	void OnAttach();
	void OnDettach();
	void OnResize();

private:
	void controlUpdate(float dt);

	std::shared_ptr<class Engine::ModelBuffer> buffer;
	std::shared_ptr<class Engine::Texture> texture;

	std::shared_ptr<class Engine::Model3D> model;

	std::shared_ptr<class Engine::Camera> ortho;
	std::shared_ptr<class Engine::Camera> perspective;

	Engine::Light light;
};