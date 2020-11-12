#pragma once

#include "Shader.h"

enum class DefaultShader
{
	Color, Texture, Skinned
};

class Renderer
{
private:
	static void Init();

public:
	static void BeginScene(class Camera& camera);
	static void Enque(DefaultShader shader, const ModelBuffer& buffer);
	static void Enque(DefaultShader shader, const ModelBuffer& buffer, const Texture& texture);
	static void EndScene();

	static class PipelineController& GetPipelineController();

	static Shader& GetDefaultShader(DefaultShader shader);
	static void CreateShader(const std::string& path, const std::string& keyName);
	static void DeleteShader(const std::string& keyName);

private:	
	friend class App;
};