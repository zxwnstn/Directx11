#pragma once

namespace Engine {

	enum class RenderMode
	{
		Deffered, Forward, Custom
	};

	enum class RenderingPath
	{
		Fbx, Deffered, Animation,
		CurvedPn, HDR, Phong
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
		static void BeginScene(std::shared_ptr<Camera> camera, const std::vector<std::shared_ptr<Light>>& lights);
		static void Enque2D(std::shared_ptr<Model2D> model);
		static void Enque3D(std::shared_ptr<Model3D> model);
		static void EndScene();
		static void Present();

		static void ActivateHdr(bool activate);
		
		static void experiment1(std::shared_ptr<Model3D> model, float factor);
		static void excompute();
		static void excompute2(std::shared_ptr<Model3D> model);
		static void exstreamout(std::shared_ptr<Model3D> model);
		static void exavlum(const std::string& texture);
		static void exhdr();
		static void computeLum();
		static void setReinhardFactor(float white, float middleGray = 0.0f);
		static float* GetReinhardFactor();

		static void SetRenderMode(RenderMode mode);
		static void Resize(uint32_t width, uint32_t height);
		
		static std::shared_ptr<struct Environment> GetGlobalEnv();
		//static RenderingData& GetData();

		static void AdjustDepthBias(int f);
		static void AdjustSlopeBias(float s);

		static void SetRenderingPath(RenderingPath path);

	private:
		static void FbxLoad();
		static void CurvedPn();
		static void HDR();
		static void Phong();
		static void Animation();
		static void Deffered();

	private:
		static void renderDeffered();
		static void renderForward();
		static void renderShadow();
		static void draw2D(std::shared_ptr<Model2D> model, const std::string& shader);
		static void draw3D(std::shared_ptr<Model3D> model, const std::string& shader);

		friend class ModuleCore;
	};

}
