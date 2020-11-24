#pragma once

#include "Shader.h"

namespace Engine {

	enum class RenderingShader
	{
		TwoDimension, StaticMesh, SkeletalMesh, Custom
	};

	std::string ToString(RenderingShader type);


	class Renderer
	{
	private:
		static void Init(const struct WindowProp& prop);
		static void prep2D();

	public:
		static void BeginScene(class Camera& camera, struct Light& light);
		static void ClearDepthStencil();
		static void Enque(std::shared_ptr<class Model3D> model, const std::string& targetTextureName = "BackBuffer");
		static void Enque(std::shared_ptr<class Model2D> model, const std::string& targetTextureName = "BackBuffer");

		static void EndScene();

		static class PipelineController& GetPipelineController();
		static void Resize(uint32_t width, uint32_t height);
		static unsigned char* GetBackBufferData();

		static Shader& GetShader(RenderingShader shader);
		static Shader& GetShader(const std::string& shader);
		static void CreateShader(const std::string& path, const std::string& keyName);
		static void DeleteShader(const std::string& keyName);
		static Environment* GetGlobalEnv();

	private:
		static void DrawStatic(std::shared_ptr<class Model3D> model);
		static void DrawSkeletal(std::shared_ptr<class Model3D> model);
		static void Draw2D(std::shared_ptr<class Model2D> model);

		friend class ModuleCore;
	};

}
