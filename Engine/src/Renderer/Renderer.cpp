#include "pch.h"

#include "Renderer.h"
#include "Dx11Core.h"
#include "PipelineController.h"
#include "Texture.h"
#include "Common/Camera.h"
#include "Common/Light.h"
#include "Model/Model.h"
#include "Model/3D/SkeletalAnimation.h"
#include "Model/3D/Skeleton.h"

namespace Engine {

	static PipelineController* s_PLController = nullptr;
	static std::unordered_map<std::string, Shader> RendererShaders;
	static Environment* GlobalEnv = nullptr;
	static std::shared_ptr<ModelBuffer> modelBuffer2D;
	static std::shared_ptr<ModelBuffer> modelBufferEffect;

	static std::vector<std::shared_ptr<Model2D>> queuedModel2D;
	static std::vector<std::shared_ptr<Model3D>> queuedModel3D;

	static std::shared_ptr<Camera> CurCamera;
	static std::shared_ptr<Light> CurLight;

	void Renderer::Init(const WindowProp& prop)
	{
		Dx11Core::Get().Init(prop);
		s_PLController = new PipelineController;
		s_PLController->Init(prop);
		GlobalEnv = new Environment;
		GlobalEnv->UseShadowMap = false;
		GlobalEnv->bias.x = 0.0000125f;
		GlobalEnv->bias.y = 0.0f;
		GlobalEnv->bias.z = 1.0f;

	}

	void Renderer::prep2D()
	{
		static float vertices[] = {
		   -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
		   -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
			0.5f,  0.5f, 0.0f, 1.0f, 0.0f
		};

		static float vertices2[] = {
		   -1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
		   -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 0.0f
		};

		static uint32_t indices[] = {
			0, 2, 1, 0, 3, 2
		};

		modelBuffer2D = RendererShaders["2D"].CreateCompotibleBuffer()
			.SetVertices(vertices, 4)
			.SetIndices(indices, 6);

		modelBufferEffect = RendererShaders["2D"].CreateCompotibleBuffer()
			.SetVertices(vertices2, 4)
			.SetIndices(indices, 6);

		Engine::TextureArchive::Add("ShadowMap", 4096, 4096, 0);
		Engine::TextureArchive::Add("SceneShadow", Dx11Core::Get().Width(), Dx11Core::Get().Height(), 0);
		Engine::TextureArchive::Add("EffectTexture1", 2048, 2048, 0);
		Engine::TextureArchive::Add("EffectTexture2", 2048, 2048, 0);
	}

	void Renderer::Enque(std::shared_ptr<Model2D> model)
	{
		queuedModel2D.push_back(model);
	}

	void Renderer::Enque(std::shared_ptr<Model3D> model)
	{
		queuedModel3D.push_back(model);
	}

	void Renderer::BeginScene(std::shared_ptr<Camera> camera, std::shared_ptr<Light> light)
	{
		s_PLController->ClearRTT();
		CurCamera = camera;
		CurLight = light;
	}

	void Renderer::EndScene()
	{
		//step 1. Create Shadow Map
		s_PLController->SetRenderTarget("ShadowMap");
		GlobalEnv->UseShadowMap = true;
		for (auto&[name, shader] : RendererShaders)
		{
			shader.SetParam<CBuffer::Camera>(CurLight->lightCam);
			shader.SetParam<CBuffer::Environment>(*GlobalEnv);
		}
		for (auto model2D : queuedModel2D) GenerateShadowMap(model2D, true);
		for (auto model3D : queuedModel3D) GenerateShadowMap(model3D, true);

		//step 2. Create Scene Shadow
		s_PLController->SetRenderTarget("SceneShadow");
		GlobalEnv->UseShadowMap = false;
		for (auto&[name, shader] : RendererShaders)
		{
			shader.SetParam<CBuffer::Camera>(*CurCamera);
			shader.SetParam<CBuffer::Environment>(*GlobalEnv);
			shader.SetParam<CBuffer::Light2>(*CurLight);
		}
		for (auto model2D : queuedModel2D) GenerateShadowMap(model2D, false);
		for (auto model3D : queuedModel3D) GenerateShadowMap(model3D, false);

		//step 3. Blur
		BlurTexture("SceneShadow");

		//step 4. Render to back buffer
		s_PLController->SetRenderTarget("BackBuffer");
		GlobalEnv->UseShadowMap = true;
		for (auto&[name, shader] : RendererShaders)
		{
			shader.SetParam<CBuffer::Environment>(*GlobalEnv);
			shader.SetParam<CBuffer::Light>(*CurLight);
		}
		for (auto model2D : queuedModel2D) Draw2D(model2D);
		for (auto model3D : queuedModel3D) Draw3D(model3D);

		queuedModel2D.clear();
		queuedModel3D.clear();

		Dx11Core::Get().Present();
	}

	void Renderer::GenerateShadowMap(std::shared_ptr<Model2D> model, bool Create)
	{
		if (!model->isActivateShadow()) 
			return;

		RendererShaders["2DShadow"].Bind();
		RendererShaders["2DShadow"].SetParam<CBuffer::Transform>(model->m_Transform);
		modelBuffer2D->Bind();

		model->m_Texture->Bind(1);
		if(!Create) TextureArchive::Get("ShadowMap")->Bind();

		Dx11Core::Get().Context->DrawIndexed(modelBuffer2D->GetIndexCount(), 0, 0);
	}

	void Renderer::GenerateShadowMap(std::shared_ptr<Model3D> model, bool Create)
	{
		if (!model->isActivateShadow()) 
			return;
		
		std::string shadowShader;
		if (model->m_Shader == "StaticMesh")
		{
			shadowShader = "StaticMeshShadow";
			RendererShaders[shadowShader].Bind();
			RendererShaders[shadowShader].SetParam<CBuffer::Transform>(model->m_Transform);
			RendererShaders[shadowShader].SetParam<CBuffer::Materials>(*model->m_MaterialSet);
		}
		if (model->m_Shader == "SkeletalMesh")
		{
			shadowShader = "SkeletalMeshShadow";
			RendererShaders[shadowShader].Bind();
			RendererShaders[model->m_Shader].SetParam<CBuffer::Bone>(model->m_Animation->MySkinnedTransforms);
			RendererShaders[shadowShader].SetParam<CBuffer::Transform>(model->m_Transform);
			RendererShaders[shadowShader].SetParam<CBuffer::Materials>(*model->m_MaterialSet);
		}

		auto& textures = model->m_MaterialSet->MaterialTextures;
		std::vector<std::string> names;
		for (int i = 0; i < textures.size(); ++i)
		{
			auto& textureSet = textures.find(i)->second;
			for (int j = 0; j < textureSet.size(); ++j)
			{
				names.push_back(textureSet[j].Name);
			}
		}
		Texture::MultipleTextureBind(names);
		model->m_ModelBuffer->Bind();
		if (!Create) TextureArchive::Get("ShadowMap")->Bind();

		Dx11Core::Get().Context->DrawIndexed(model->m_ModelBuffer->GetIndexCount(), 0, 0);
	}
	
	void Renderer::Draw2D(std::shared_ptr<Model2D> model)
	{
		RendererShaders[model->m_Shader].Bind();
		RendererShaders[model->m_Shader].SetParam<CBuffer::Transform>(model->m_Transform);
		modelBuffer2D->Bind();

		model->m_Texture->Bind(1);
		TextureArchive::Get("SceneShadow")->Bind(0);

		Dx11Core::Get().Context->DrawIndexed(modelBuffer2D->GetIndexCount(), 0, 0);
	}

	void Renderer::Draw3D(std::shared_ptr<Model3D> model)
	{
		if (model->m_Shader == "StaticMesh") DrawStatic(model);
		if (model->m_Shader == "SkeletalMesh") DrawSkeletal(model);
	}

	void Renderer::DrawStatic(std::shared_ptr<Model3D> model)
	{
		RendererShaders[model->m_Shader].Bind();
		RendererShaders[model->m_Shader].SetParam<CBuffer::Transform>(model->m_Transform);
		RendererShaders[model->m_Shader].SetParam<CBuffer::Materials>(*model->m_MaterialSet);

		auto& textures = model->m_MaterialSet->MaterialTextures;
		std::vector<std::string> names;
		for (int i = 0; i < textures.size(); ++i)
		{
			auto& textureSet = textures.find(i)->second;
			for (int j = 0; j < textureSet.size(); ++j)
			{
				names.push_back(textureSet[j].Name);
			}
		}
		TextureArchive::Get("SceneShadow")->Bind(0);
		Texture::MultipleTextureBind(names, 1);
		model->m_ModelBuffer->Bind();

		Dx11Core::Get().Context->DrawIndexed(model->m_ModelBuffer->GetIndexCount(), 0, 0);
	}

	void Renderer::DrawSkeletal(std::shared_ptr<Model3D> model)
	{
		RendererShaders[model->m_Shader].Bind();
		RendererShaders[model->m_Shader].SetParam<CBuffer::Bone>(model->m_Animation->MySkinnedTransforms);
		RendererShaders[model->m_Shader].SetParam<CBuffer::Transform>(model->m_Transform);
		RendererShaders[model->m_Shader].SetParam<CBuffer::Materials>(*model->m_MaterialSet);

		auto& textures = model->m_MaterialSet->MaterialTextures;
		std::vector<std::string> names;
		for (int i = 0; i < textures.size(); ++i)
		{
			auto& textureSet = textures.find(i)->second;
			for (int j = 0; j < textureSet.size(); ++j)
			{
				names.push_back(textureSet[j].Name);
			}
		}
		TextureArchive::Get("SceneShadow")->Bind(0);
		Texture::MultipleTextureBind(names);
		model->m_ModelBuffer->Bind();

		Dx11Core::Get().Context->DrawIndexed(model->m_ModelBuffer->GetIndexCount(), 0, 0);
	}

	PipelineController& Renderer::GetPipelineController()
	{
		return *s_PLController;
	}

	void Renderer::Resize(uint32_t width, uint32_t height)
	{
		LOG_MISC("Renderer::Resize window {0} {1}", width, height);
		Dx11Core::Get().Resize(width, height);
	}

	void Renderer::BlurTexture(const std::string & textureName)
	{
		auto target = Engine::TextureArchive::Get(textureName);

		
		auto effect1 = TextureArchive::Get("EffectTexture1");
		s_PLController->SetRenderTarget("EffectTexture1");
		RendererShaders["VerticalBlur"].Bind();
		RendererShaders["VerticalBlur"].SetParam<CBuffer::TextureInform>(target);
		target->Bind();
		modelBufferEffect->Bind();
		Dx11Core::Get().Context->DrawIndexed(modelBufferEffect->GetIndexCount(), 0, 0);

		auto effect2 = TextureArchive::Get("EffectTexture2");
		s_PLController->SetRenderTarget("EffectTexture2");
		RendererShaders["HorizontalBlur"].Bind();
		RendererShaders["HorizontalBlur"].SetParam<CBuffer::TextureInform>(target);
		modelBufferEffect->Bind();
		effect1->Bind();
		Dx11Core::Get().Context->DrawIndexed(modelBufferEffect->GetIndexCount(), 0, 0);

		s_PLController->SetRenderTarget(textureName);
		RendererShaders["Copy"].Bind();
		modelBufferEffect->Bind();
		effect2->Bind();
		Dx11Core::Get().Context->DrawIndexed(modelBufferEffect->GetIndexCount(), 0, 0);
	}

	Shader& Renderer::GetShader(RenderingShader shader)
	{
		return RendererShaders[ToString(shader)];
	}

	Shader & Renderer::GetShader(const std::string & shader)
	{
		return RendererShaders[shader];
	}

	void Renderer::CreateShader(const std::string & path, const std::string & keyName)
	{
		auto find = RendererShaders.find(keyName);
		if (find != RendererShaders.end())
			return;

		RendererShaders.emplace(keyName, path);
	}

	void Renderer::DeleteShader(const std::string & keyName)
	{
		auto find = RendererShaders.find(keyName);
		if (find == RendererShaders.end())
			return;

		RendererShaders.erase(find);
	}

	Environment * Renderer::GetGlobalEnv()
	{
		return GlobalEnv;
	}

	std::string ToString(RenderingShader type)
	{
		switch (type)
		{
		case RenderingShader::SkeletalMesh: return "SkeletalMesh";
		case RenderingShader::StaticMesh: return "StaticMesh";
		case RenderingShader::TwoDimension: return "2D";
		case RenderingShader::VerticalBlur: return "VerticalBlur";
		case RenderingShader::HorizontalBlur: return "HorizontalBlur";
		}
		return "";
	}

}
