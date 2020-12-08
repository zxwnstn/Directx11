#pragma once

namespace Engine {

	enum class RenderMode
	{
		Deffered, Forward, Custom
	};

	struct Light;
	class Camera;
	class Model2D;
	class Model3D;
	
	class Renderer
	{
	private:
		static void Init(const struct WindowProp& prop);
		static void Shutdown();

	public:
		static void BeginScene(std::shared_ptr<Camera> camera, const std::initializer_list<std::shared_ptr<Light>>& lights);
		static void Enque2D(std::shared_ptr<Model2D> model);
		static void Enque3D(std::shared_ptr<Model3D> model);
		static void EndScene();

		static void SetRenderMode(RenderMode mode);
		static void Resize(uint32_t width, uint32_t height);
		
		static std::shared_ptr<struct Environment> GetGlobalEnv();
		//static RenderingData& GetData();

		static void AdjustDepthBias(int f);
		static void AdjustSlopeBias(float s);

	private:
		static void renderDeffered();
		static void renderForward();
		static void renderShadow();
		static void draw2D(std::shared_ptr<Model2D> model, const std::string& shader);
		static void draw3D(std::shared_ptr<Model3D> model, const std::string& shader);

		friend class ModuleCore;
	};

}
