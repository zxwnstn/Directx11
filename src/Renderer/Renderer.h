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
	static void Init();

public:
	static void BeginScene(class Camera& camera);
	static void Enque(RenderingShader shader, const ModelBuffer& buffer);
	static void Enque(RenderingShader shader, const ModelBuffer& buffer, const class Texture& texture);
	static void EndScene();

	static class PipelineController& GetPipelineController();

	static Shader& GetShader(RenderingShader shader);
	static void CreateShader(const std::string& path, const std::string& keyName);
	static void DeleteShader(const std::string& keyName);

private:	
	friend class App;
};