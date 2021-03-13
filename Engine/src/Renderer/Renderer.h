#pragma once

namespace Engine {

	enum class RenderMode
	{
		Deffered, Forward, Custom
	};

	enum class RenderingPath
	{
		Forward, Deffered
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
		static void ActivateHdr(bool activate);
		static void ActivateShadow(bool activate);

	public:
		static void BeginScene(std::shared_ptr<Camera> camera, const std::vector<std::shared_ptr<Light>>& lights);
		static void Enque2D(std::shared_ptr<Model2D> model);
		static void Enque3D(std::shared_ptr<Model3D> model);
		static void EndScene();
		static void Present();

		static void experiment1(std::shared_ptr<Model3D> model, float factor);
		static void excompute();
		static void excompute2(std::shared_ptr<Model3D> model);
		static void exstreamout(std::shared_ptr<Model3D> model);
		static void exavlum(const std::string& texture);
		static void exhdr();

	public:
		static std::shared_ptr<struct Environment> GetGlobalEnv();
		static float* GetReinhardFactor();

		static void SetRenderMode(RenderMode mode);
		static void AdjustDepthBias(int f);
		static void AdjustSlopeBias(float s);
		static void Resize(uint32_t width, uint32_t height);

	private:
		static float computeLum(const std::string& target);
		static void renderDeffered();
		static void renderForward();
		static void renderShadow();
		static void renderLight(const std::shared_ptr<Light>& light);
		static void draw2D(std::shared_ptr<Model2D> model, const std::string& shader);
		static void draw3D(std::shared_ptr<Model3D> model, const std::string& shader, int materialBind = 0);

		static void Some();

		friend class ModuleCore;
	};

}
