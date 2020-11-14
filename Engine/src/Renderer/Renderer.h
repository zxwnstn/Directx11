#pragma once

#include "Shader.h"

enum class RenderingShader
{
	Color, Texture, Skinned, Custum
};

std::string ToString(RenderingShader type);


class Renderer
{
private:
	static void Init(const struct WindowProp& prop);

public:
	static void BeginScene(class Camera& camera);
	static void Enque(RenderingShader shader, const ModelBuffer& buffer);
	static void Enque(RenderingShader shader, const ModelBuffer& buffer, const class Texture& texture);
	static void Enque(std::shared_ptr<class Model3D> model);
	static void Enque(std::shared_ptr<class Model2D> model);
	static void EndScene();

	static class PipelineController& GetPipelineController();

	static Shader& GetShader(RenderingShader shader);
	static Shader& GetShader(const std::string& shader);
	static void CreateShader(const std::string& path, const std::string& keyName);
	static void DeleteShader(const std::string& keyName);

private:
	friend class ModuleCore;
};