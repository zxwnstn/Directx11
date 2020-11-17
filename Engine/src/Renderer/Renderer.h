#pragma once

#include "Shader.h"

namespace Engine {

	enum class RenderingShader
	{
		Color, Texture, Skinned, Lighting, Custum
	};

	std::string ToString(RenderingShader type);


	class Renderer
	{
	private:
		static void Init(const struct WindowProp& prop);

	public:
		static void BeginScene(class Camera& camera, struct Light& light);
		static void Enque(std::shared_ptr<class Model3D> model);
		static void Enque(std::shared_ptr<class Model2D> model);
		static void Enque(std::shared_ptr<ModelBuffer> buffer, RenderingShader type);
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
		friend class ModuleCore;
	};

}
