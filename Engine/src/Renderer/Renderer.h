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
		static void ActivateGamma(bool activate);
		static void ActivateShowGBuffer(bool activate);
		static void ActivateWire(bool activate);
		static void ActivateLighting(bool activate);
		static void SetTFactor(float tFactor);

	public:
		static void BeginScene(std::shared_ptr<Camera> camera, const std::vector<std::shared_ptr<Light>>& lights);
		static void Enque2D(std::shared_ptr<Model2D> model);
		static void Enque3D(std::shared_ptr<Model3D> model);
		static void EndScene();
		static void Present();

	public:
		static std::shared_ptr<struct Environment> GetGlobalEnv();
		static float* GetReinhardFactor();

		static void SetRenderMode(RenderMode mode);
		static void AdjustShadowBias(float depth, float slope);
		static void Resize(uint32_t width, uint32_t height);

	private:
		static float computeLum(const std::string& target);
		static void renderDeffered();
		static void renderForward();
		static void renderShadow();
		static void renderGBuffer();
		static void renderLight(const std::shared_ptr<Light>& light);
		static void draw2D(std::shared_ptr<Model2D> model, const std::string& shader);
		static void draw3D(std::shared_ptr<Model3D> model, const std::string& shader, int materialBind = 0, bool tess = false);

		static void Some();

		friend class ModuleCore;
	};

}
