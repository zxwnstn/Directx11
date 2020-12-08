#pragma once

class SandBox 
{
public:
	void OnUpdate(float dt);
	void OnAttach();
	void OnDettach();
	void OnResize();
	void OnMouseMove(float dx, float dy);

private:
	void controlUpdate(float dt);

	std::shared_ptr<class Engine::Model3D> fbxmodel;
	std::shared_ptr<class Engine::Model3D> objmodel;
	std::shared_ptr<class Engine::Model3D> floor;
	std::shared_ptr<class Engine::Model2D> debugwindow;

	std::shared_ptr<class Engine::Camera> ortho;
	std::shared_ptr<class Engine::Camera> perspective;

	std::shared_ptr<struct Engine::Light> light;
	std::shared_ptr<struct Engine::Light> light2;

	float mouseSensitive = 0.005f;
};