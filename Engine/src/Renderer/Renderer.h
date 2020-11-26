#pragma once

#include "Shader.h"

namespace Engine {

	enum class RenderingShader
	{
		TwoDimension, StaticMesh, SkeletalMesh, VerticalBlur, HorizontalBlur, Custom
	};

	std::string ToString(RenderingShader type);

	class Model2D;
	class Model3D;
	struct Light;

	class Renderer
	{
	private:
		static void Init(const struct WindowProp& prop);
		static void prep2D();

	public:
		static void BeginScene(std::shared_ptr<Camera> camera, std::shared_ptr<Light> light);
		static void Enque(std::shared_ptr<Model3D> model);
		static void Enque(std::shared_ptr<Model2D> model);
		static void EndScene();

		static class PipelineController& GetPipelineController();
		static void Resize(uint32_t width, uint32_t height);
		static void BlurTexture(const std::string& textureName);

		static Shader& GetShader(RenderingShader shader);
		static Shader& GetShader(const std::string& shader);
		static void CreateShader(const std::string& path, const std::string& keyName);
		static void DeleteShader(const std::string& keyName);
		static Environment* GetGlobalEnv();

	private:
		static void GenerateShadowMap(std::shared_ptr<Model3D> model, bool Create);
		static void GenerateShadowMap(std::shared_ptr<Model2D> model, bool Create);

		static void Draw2D(std::shared_ptr<Model2D> model);
		static void Draw3D(std::shared_ptr<Model3D> model);
		static void DrawStatic(std::shared_ptr<Model3D> model);
		static void DrawSkeletal(std::shared_ptr<Model3D> model);

		friend class ModuleCore;
	};

}
