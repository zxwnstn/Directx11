#include "pch.h"

#include "Renderer.h"
#include "Dx11Core.h"
#include "Shader.h"
#include "PipelineController.h"
#include "Texture.h"
#include "Common/Camera.h"
#include "Common/Light.h"
#include "Model/Model.h"
#include "ModelBuffer.h"
#include "Model/3D/SkeletalAnimation.h"
#include "Model/3D/Skeleton.h"
#include "GBuffer.h"

#define MAX_LIGHT 100

namespace Engine {
	
	struct RenderingData {

		std::shared_ptr<PipelineController> PLController;
		std::shared_ptr<Environment> GlobalEnv;
		std::shared_ptr<GBuffer> GeometryBuffer;

		std::vector<std::shared_ptr<ShadowMap>> SpotShadowMaps;
		std::vector<std::shared_ptr<ShadowMap>> PointShadowMaps;

		std::shared_ptr<ModelBuffer> ModelBuffer2D;
		std::shared_ptr<Model2D> SkyCube;

		std::vector<std::shared_ptr<Model2D>> Queued2D;
		std::vector<std::shared_ptr<Model3D>> Queued3D;
		std::vector<std::shared_ptr<Light>>   QueuedLight;
		std::shared_ptr<Camera>	ActiveCamera;

		RenderMode Mode;

	}static s_Data;

	void Renderer::Init(const WindowProp& prop)
	{
		Dx11Core::Get().Init(prop);

		s_Data.PLController.reset(new PipelineController);
		s_Data.PLController->Init(prop);
		s_Data.PLController->SetBlend(BlendOpt::Alpha)
			.SetDepthStencil(DepthStencilOpt::Enable)
			.SetRasterize(RasterlizerOpt::Solid);

		s_Data.GlobalEnv.reset(new Environment);
		s_Data.GlobalEnv->UseShadowMap = false;
		s_Data.GlobalEnv->bias.x = 0.0000125f;
		s_Data.GlobalEnv->bias.y = 0.0f;
		s_Data.GlobalEnv->bias.z = 1.0f;
		s_Data.GlobalEnv->Ambient = { 0.8f, 0.8f, 0.8f };

		s_Data.GeometryBuffer.reset(new GBuffer(Dx11Core::Get().Width(), Dx11Core::Get().Height(), {
			{"Diffuse", DXGI_FORMAT_R32G32B32A32_FLOAT}, {"Normal",  DXGI_FORMAT_R32G32B32A32_FLOAT},
			{"Ambient", DXGI_FORMAT_R32G32B32A32_FLOAT}, {"WorldPosition", DXGI_FORMAT_R32G32B32A32_FLOAT},
			{"Misc", DXGI_FORMAT_R8G8B8A8_UNORM},
		}));

		float vertices[] = {
		   -1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
		   -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 0.0f
		};

		static uint32_t indices[] = {
			0, 2, 1, 0, 3, 2
		};

		s_Data.ModelBuffer2D = ModelBuffer::Create(MeshType::Sqaure)
			.SetVertices(vertices, 4)
			.SetIndices(indices, 6);

		s_Data.Mode = RenderMode::Deffered;

		for (uint32_t i = 0; i < MAX_LIGHT; ++i)
			s_Data.SpotShadowMaps.emplace_back(new ShadowMap(2048, 2048));
		for (uint32_t i = 0; i < 10; ++i)
			s_Data.PointShadowMaps.emplace_back(new ShadowMap(4096, 4096, 6));
	}

	void Renderer::Shutdown()
	{
	}

	void Renderer::BeginScene(std::shared_ptr<Camera> camera, const std::initializer_list<std::shared_ptr<Light>>& lights)
	{
		s_Data.PLController->ClearRTT();
		s_Data.ActiveCamera = camera;
		s_Data.QueuedLight.assign(lights);
	}

	void Renderer::Enque2D(std::shared_ptr<Model2D> model)
	{
		s_Data.Queued2D.push_back(model);
	}

	void Renderer::Enque3D(std::shared_ptr<Model3D> model)
	{
		s_Data.Queued3D.push_back(model);
	}

	void Renderer::EndScene()
	{
		for (auto&[name, shader] : ShaderArchive::s_Shaders)
			shader->SetParam<CBuffer::Environment>(*s_Data.GlobalEnv);

		renderShadow();
		
		switch (s_Data.Mode)
		{
		case RenderMode::Deffered: renderDeffered();
		case RenderMode::Forward: renderForward();
		}

		s_Data.PLController->SetDepthStencil(DepthStencilOpt::Disable);

		//Render 2D
		s_Data.PLController->SetBlend(BlendOpt::None);
		for (auto model : s_Data.Queued2D) draw2D(model, "2D");

		s_Data.Queued2D.clear();
		s_Data.Queued3D.clear();
		s_Data.QueuedLight.clear();

		Dx11Core::Get().Present();
	}

	void Renderer::renderShadow()
	{
		Dx11Core::Get().Context->PSSetShader(nullptr, nullptr, 0);

		s_Data.PLController->SetRasterize(RasterlizerOpt::Shadow);
		s_Data.PLController->SetDepthStencil(DepthStencilOpt::Enable);

		auto SpotStatic = ShaderArchive::Get("EffectShadowSpotStatic");
		auto SpotSkeletal = ShaderArchive::Get("EffectShadowSpotSkeletal");
		auto PointStatic = ShaderArchive::Get("EffectShadowPointStatic");
		auto PointSkeletal = ShaderArchive::Get("EffectShadowPointSkeletal");

		uint32_t spotIndex = 0;
		uint32_t pointIndex = 0;
		for (auto light : s_Data.QueuedLight)
		{
			if (light->m_Type == Light::Type::Directional)
			{
				//for (auto model2D : s_Data.Queued2D) draw2D(model2D, "EffectShadowDirectional");
				//for (auto model3D : s_Data.Queued3D) draw3D(model3D, "EffectShadowDirectional");
			}
			if (light->m_Type == Light::Type::Point)
			{
				s_Data.PointShadowMaps[pointIndex++]->SetRenderTarget();

				PointStatic->SetParam<CBuffer::CubeCamera>(light->lightCam);
				PointSkeletal->SetParam<CBuffer::CubeCamera>(light->lightCam);
				for (auto model3D : s_Data.Queued3D) draw3D(model3D, "EffectShadowPoint");
			}
			if (light->m_Type == Light::Type::Spot)
			{
				s_Data.SpotShadowMaps[spotIndex++]->SetRenderTarget();

				SpotStatic->SetParam<CBuffer::Camera>(light->lightCam);
				SpotSkeletal->SetParam<CBuffer::Camera>(light->lightCam);
				for (auto model3D : s_Data.Queued3D) draw3D(model3D, "EffectShadowSpot");
			}
		}
	}

	void Renderer::renderDeffered()
	{
		//Pass1. Render Geometry Buffer
		s_Data.GeometryBuffer->SetRenderTarget();
		s_Data.PLController->SetDepthStencil(DepthStencilOpt::Enable);
		s_Data.PLController->SetRasterize(RasterlizerOpt::Solid);
		s_Data.PLController->SetBlend(BlendOpt::Alpha);
		for (auto&[name, shader] : ShaderArchive::s_Shaders)
			shader->SetParam<CBuffer::Camera>(*s_Data.ActiveCamera);

		std::string shader;
		switch (s_Data.Mode)
		{
		case RenderMode::Deffered: shader = "Deffered"; break;
		case RenderMode::Forward: shader = "Forward"; break;
		}
		for (auto model3D : s_Data.Queued3D) draw3D(model3D, shader);
		
		//Pass2. Render light with gbuffer
		s_Data.PLController->SetRenderTarget("BackBuffer");
		s_Data.PLController->SetDepthStencil(DepthStencilOpt::Disable);
		s_Data.PLController->SetBlend(BlendOpt::GBuffer);

		auto lightingShader = ShaderArchive::Get("DefferedLighting");
		lightingShader->Bind();
		s_Data.ModelBuffer2D->Bind();
		s_Data.GeometryBuffer->Bind();

		uint32_t pointIndex = 0;
		uint32_t spotIndex = 0;
		for (uint32_t i = 0; i < s_Data.QueuedLight.size(); ++i)
		{
			lightingShader->SetParam<CBuffer::Light>(*s_Data.QueuedLight[i]);
			lightingShader->SetParam<CBuffer::LightCam>(*s_Data.QueuedLight[i]);
			switch (s_Data.QueuedLight[i]->m_Type)
			{
			case Light::Type::Directional:
				break;
			case Light::Type::Point:
				Dx11Core::Get().Context->PSSetShaderResources(7, 1, &s_Data.PointShadowMaps[pointIndex]->m_ShaderResourceView);
				pointIndex++;
				break;
			case Light::Type::Spot:
				Dx11Core::Get().Context->PSSetShaderResources(6, 1, &s_Data.SpotShadowMaps[spotIndex]->m_ShaderResourceView);
				spotIndex++;
				break;
			}
			Dx11Core::Get().Context->DrawIndexed(s_Data.ModelBuffer2D->GetIndexCount(), 0, 0);
		}	
	}

	void Renderer::renderForward()
	{
	}

	void Renderer::draw2D(std::shared_ptr<Model2D> model, const std::string& shader)
	{
		auto myShader = ShaderArchive::Get("2D");
		myShader->Bind();

		myShader->SetParam<CBuffer::Transform>(model->m_Transform);
		s_Data.ModelBuffer2D->Bind();
		//model->m_Texture->Bind(0);
		//Dx11Core::Get().Context->PSSetShaderResources(0, 1, &s_Data.SpotShadowMaps[0]->m_ShaderResourceView);

		Dx11Core::Get().Context->DrawIndexed(s_Data.ModelBuffer2D->GetIndexCount(), 0, 0);
	}

	void Renderer::draw3D(std::shared_ptr<Model3D> model, const std::string& shader)
	{
		std::string useShader = shader;
		switch (model->m_ModelBuffer->GetMeshType())
		{
		case MeshType::Skeletal: useShader += "Skeletal"; break;
		case MeshType::Static: useShader += "Static"; break;
		}

		auto myShader = ShaderArchive::Get(useShader);
		myShader->Bind();
		
		myShader->SetParam<CBuffer::Transform>(model->m_Transform);
		myShader->SetParam<CBuffer::Materials>(*model->m_MaterialSet);
		myShader->SetParam<CBuffer::Bone>(model->m_Animation->MySkinnedTransforms);
		
		model->m_ModelBuffer->Bind();

		if(myShader->Has(Shader::Type::PixelShader))
			model->m_MaterialSet->BindTextures(0);

		Dx11Core::Get().Context->DrawIndexed(model->m_ModelBuffer->GetIndexCount(), 0, 0);
		myShader->Unbind();
	}

	void Renderer::SetRenderMode(RenderMode mode)
	{
		s_Data.Mode = mode;
	}

	void Renderer::Resize(uint32_t width, uint32_t height)
	{
		LOG_MISC("Renderer::Resize window {0} {1}", width, height);
		Dx11Core::Get().Resize(width, height);
		s_Data.GeometryBuffer->Resize(width, height);
	}

	std::shared_ptr<Environment> Renderer::GetGlobalEnv()
	{
		return s_Data.GlobalEnv;
	}

	void Renderer::AdjustDepthBias(int f)
	{
		s_Data.PLController->m_Rasterlizer.AdjustShadowBias(f, 0.0f);
		LOG_TRACE("Current Depth Bias : {0}", s_Data.PLController->m_Rasterlizer.RasterDesc.DepthBias);
	}

	void Renderer::AdjustSlopeBias(float s)
	{
		s_Data.PLController->m_Rasterlizer.AdjustShadowBias(0, s);
		LOG_TRACE("Current Slope Bias : {0}", s_Data.PLController->m_Rasterlizer.RasterDesc.SlopeScaledDepthBias);
	}

}
