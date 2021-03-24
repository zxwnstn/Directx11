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
#include "SBuffer.h"
#include "Core/ModuleCore.h"

#define MAX_LIGHT 100

namespace Engine {

	struct RenderingData {

		std::shared_ptr<PipelineController> PLController;
		std::shared_ptr<Environment> GlobalEnv;
		std::shared_ptr<GBuffer> GeometryBuffer;

		std::vector<std::shared_ptr<ShadowMap>> SpotShadowMaps;
		std::vector<std::shared_ptr<ShadowMap>> PointShadowMaps;
		std::vector<std::shared_ptr<ShadowMap>> DirShadowMaps;
		std::vector<std::shared_ptr<Texture>> EnvironmentMaps;

		std::shared_ptr<ModelBuffer> ModelBuffer2D;
		std::shared_ptr<Model2D> SkyCube;

		std::vector<std::shared_ptr<Model2D>> Queued2D;
		std::vector<std::shared_ptr<Model3D>> Queued3D;
		std::vector<std::shared_ptr<Light>>   QueuedLight;
		std::shared_ptr<Camera>	ActiveCamera;

		float ReinhardToneMapFactor[3];
		std::shared_ptr<SBuffer<float>> LumCsBuffer;
		std::shared_ptr<Light> emptyLight;
		std::shared_ptr<Model3D> lightModel;
		vec3 SkyColor;

		ShadingData ShadingData_;

		RenderMode Mode;
		RenderingPath Path;

	}static s_Data;

	bool isHdr = false;
	bool isGamma = false;
	bool showGBuffer = false;
	bool isWire = false;
	bool isShadow = false;
	bool isLighting = false;
	float tFactor = 1.0f;
	bool isMinimized = false;
	bool realtimeEnvironment = false;

	std::shared_ptr<Environment> Renderer::GetGlobalEnv()
	{
		return s_Data.GlobalEnv;
	}

	void Renderer::AdjustShadowBias(int depth, float slope)
	{
		s_Data.PLController->m_Rasterlizer.AdjustShadowBias(depth, slope);
	}

	void Renderer::ActivateHdr(bool activate)
	{
		isHdr = activate;
	}

	void Renderer::ActivateShadow(bool activate)
	{
		isShadow = activate;
		s_Data.ShadingData_.Data1.y = (int)activate;
	}

	void Renderer::SetSpecularMode(int mode)
	{
		s_Data.ShadingData_.Data1.w = mode;
	}

	void Renderer::SetDiffuseMode(int mode)
	{
		s_Data.ShadingData_.Data1.z = mode;
	}

	void Renderer::SetLambertContrast(int factor)
	{
		s_Data.ShadingData_.Data2.x = factor;
	}

	void Renderer::ActivateGamma(bool activate)
	{
		s_Data.ShadingData_.Data2.z = (int)activate;
	}

	void Renderer::ActivateShowGBuffer(bool activate)
	{
		showGBuffer = activate;
	}

	void Renderer::ActivateWire(bool activate)
	{
		isWire = activate;
	}

	void Renderer::SetTFactor(float factor)
	{
		tFactor = factor;
	}

	void Renderer::AppMinimized(bool min)
	{
		isMinimized = min;
	}

	vec3 & Renderer::GetSkyColor()
	{
		return s_Data.SkyColor;
	}

	void Renderer::SetRenderMode(RenderMode mode)
	{
		s_Data.Mode = mode;
	}

	void Renderer::ActivateLighting(bool activate)
	{
		isLighting = activate;
		s_Data.ShadingData_.Data1.x = (int)activate;
	}

	float* Renderer::GetReinhardFactor()
	{
		return s_Data.ReinhardToneMapFactor;
	}

	void Renderer::ActivateVSync(bool activate)
	{
		Dx11Core::Get().SetVSync(activate);
	}

	void Renderer::ActivateRealTimeEnvironment(bool activate)
	{
		realtimeEnvironment = activate;
	}

	void Renderer::Init(const WindowProp& prop)
	{
		Dx11Core::Get().Init(prop);

		s_Data.PLController.reset(new PipelineController);
		s_Data.PLController->Init(prop);
		s_Data.PLController->SetBlend(BlendOpt::Alpha)
			.SetDepthStencil(DepthStencilOpt::Enable)
			.SetRasterize(RasterlizerOpt::Solid);

		s_Data.GlobalEnv.reset(new Environment);
		s_Data.GeometryBuffer.reset(new GBuffer(Dx11Core::Get().Width(), Dx11Core::Get().Height(), {
			{"Diffuse", DXGI_FORMAT_R32G32B32A32_FLOAT}, {"Normal",  DXGI_FORMAT_R32G32B32A32_FLOAT},
			{"Ambient", DXGI_FORMAT_R32G32B32A32_FLOAT}, {"WorldPosition", DXGI_FORMAT_R32G32B32A32_FLOAT},
			{"Specular", DXGI_FORMAT_R32G32B32A32_FLOAT}, {"Misc", DXGI_FORMAT_R8G8B8A8_UNORM},
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

		s_Data.Mode = RenderMode::Forward;

		for (uint32_t i = 0; i < 10; ++i)
			s_Data.SpotShadowMaps.emplace_back(new ShadowMap(4096, 4096));
		for (uint32_t i = 0; i < 10; ++i)
			s_Data.PointShadowMaps.emplace_back(new ShadowMap(4096, 4096, 6));
		for (uint32_t i = 0; i < 10; ++i)
			s_Data.DirShadowMaps.emplace_back(new ShadowMap(4096, 4096, 4));

		TextureArchive::Add("HDRTexture", prop.Width, prop.Height);
		TextureArchive::Add("ForwardTexture", prop.Width, prop.Height);

		s_Data.LumCsBuffer.reset(new SBuffer<float>(SBufferType::Write, nullptr, 20000));

		s_Data.emptyLight.reset(new Light);
		s_Data.emptyLight->noLight = true;

		s_Data.ReinhardToneMapFactor[0] = 6.0f;
		s_Data.ReinhardToneMapFactor[1] = 6.0f;

		for (int i = 0; i < 50; ++i)
		{
			s_Data.EnvironmentMaps.push_back(std::shared_ptr<Texture>(new Texture(1024u, 1024u, 6u, true)));
		}

	}

	void Renderer::Shutdown()
	{
	}

	void Renderer::Resize(uint32_t width, uint32_t height)
	{
		LOG_MISC("Renderer::Resize window {0} {1}", width, height);

		Dx11Core::Get().Resize(width, height);
		s_Data.GeometryBuffer->Resize(width, height);

		TextureArchive::Get("ForwardTexture")->Resize(width, height);
		TextureArchive::Get("HDRTexture")->Resize(width, height);
	}


	void Renderer::Enque2D(std::shared_ptr<Model2D> model)
	{
		s_Data.Queued2D.push_back(model);
	}

	void Renderer::Enque3D(std::shared_ptr<Model3D> model)
	{
		s_Data.Queued3D.push_back(model);
	}

	void Renderer::BeginScene(std::shared_ptr<Camera> camera, const std::vector<std::shared_ptr<Light>>& lights)
	{
		s_Data.PLController->ClearRTT();
		s_Data.ActiveCamera = camera;
		s_Data.QueuedLight = lights;
	}

	void Renderer::EndScene()
	{
		if (isMinimized)
		{
			s_Data.Queued2D.clear();
			s_Data.Queued3D.clear();
			s_Data.QueuedLight.clear();
			return;
		}

		renderShadow();
		renderEnvironment();

		switch (s_Data.Mode)
		{
		case RenderMode::Deffered: renderDeffered(); break;
		case RenderMode::Forward: renderForward(); break;
		}

		//Render 2D
		s_Data.PLController->SetDepthStencil(DepthStencilOpt::Disable);
		s_Data.PLController->SetBlend(BlendOpt::None);
		for (auto model : s_Data.Queued2D)
			draw2D(model, "2D");

		if (showGBuffer)
			renderGBuffer();

		//Some();

		s_Data.Queued2D.clear();
		s_Data.Queued3D.clear();
		s_Data.QueuedLight.clear();
	}

	void Renderer::Present()
	{
		Dx11Core::Get().Present();
	}

	void Renderer::renderSkyBox()
	{
		for (auto it = s_Data.Queued3D.begin(); it != s_Data.Queued3D.end();)
		{
			if ((*it)->GetMeshType() == 2)
			{
				if (s_Data.Mode == RenderMode::Forward)
				{
					draw3D(*it, "Forward", 3, true);
					it = s_Data.Queued3D.erase(it);
				}
				else
				{
					draw3D(*it, "Deffered", 3, true);
					it = s_Data.Queued3D.erase(it);
				}
			}
			else
			{
				++it;
			}
		}
	}

	void Renderer::renderEnvironment()
	{
		if (!realtimeEnvironment) return;

		int i = 0;
		for (auto model : s_Data.Queued3D)
		{
			bool isReflect = false;
			auto& mats = model->m_MaterialSet->Materials;
			for (auto material : mats)
			{
				if (material.second.Roughness != 1.0f && material.second.Metalic != 0.0f)
				{
					isReflect = true;
					break;
				}
			}
			if (!isReflect) continue;

			s_Data.EnvironmentMaps[i]->SetAsRenderTarget();
			for (auto anotherModel : s_Data.Queued3D)
			{
				if (anotherModel == model) continue;
				draw3D(anotherModel, "EffectEnvironment");
			}
		}
	}

	void Renderer::renderShadow()
	{
		if (!isShadow) return;

		Dx11Core::Get().Context->PSSetShader(nullptr, nullptr, 0);

		s_Data.PLController->SetRasterize(RasterlizerOpt::Shadow);
		s_Data.PLController->SetDepthStencil(DepthStencilOpt::Enable);

		auto SpotStatic = ShaderArchive::Get("EffectShadowSpotStatic");
		auto SpotSkeletal = ShaderArchive::Get("EffectShadowSpotSkeletal");
		auto PointStatic = ShaderArchive::Get("EffectShadowPointStatic");
		auto PointSkeletal = ShaderArchive::Get("EffectShadowPointSkeletal");
		auto DirStatic = ShaderArchive::Get("EffectShadowDirectionalStatic");
		auto DirSkeletal = ShaderArchive::Get("EffectShadowDirectionalSkeletal");

		uint32_t spotIndex = 0;
		uint32_t pointIndex = 0;
		uint32_t dirIndex = 0;
		std::string shadername;
		for (auto light : s_Data.QueuedLight)
		{
			switch (light->m_Type)
			{
			case Light::Type::Directional: 
				shadername = "EffectShadowDirectional";
				light->UpdateCascade(s_Data.ActiveCamera);
				DirStatic->SetParam<CBuffer::CascadedViewProj>(light->m_CascadedMat);
				DirSkeletal->SetParam<CBuffer::CascadedViewProj>(light->m_CascadedMat);
				s_Data.DirShadowMaps[dirIndex++]->SetRenderTarget();
				break;
			case Light::Type::Point:
				shadername = "EffectShadowPoint";
				PointStatic->SetParam<CBuffer::CubeCamera>(light->lightCam);
				PointSkeletal->SetParam<CBuffer::CubeCamera>(light->lightCam);
				s_Data.PointShadowMaps[pointIndex++]->SetRenderTarget();
				break;
			case Light::Type::Spot:
				shadername = "EffectShadowSpot";
				SpotStatic->SetParam<CBuffer::LightCam>(*light);
				SpotSkeletal->SetParam<CBuffer::LightCam>(*light);
				s_Data.SpotShadowMaps[spotIndex++]->SetRenderTarget();
				break;
			}
			for (auto model3D : s_Data.Queued3D)
			{
				if (model3D->GetMeshType() == 2) continue;
				draw3D(model3D, shadername);
			}
		}
	}

	void Renderer::renderGBuffer()
	{
		int i = 0;
		for (auto buffer : s_Data.GeometryBuffer->m_BindingOrder)
		{
			auto myShader = ShaderArchive::Get("2D");
			myShader->Bind();
			Transform t;
			t.SetScale(0.15f, 0.2f, 1.0f);
			t.SetTranslate(i * 0.3f - 0.85f, -0.8f, 0.0f);
			myShader->SetParam<CBuffer::Transform>(t);
			s_Data.ModelBuffer2D->Bind();
			Dx11Core::Get().Context->PSSetShaderResources(0, 1, &s_Data.GeometryBuffer->m_Buffers[buffer]->m_ResourceView);
			Dx11Core::Get().Context->DrawIndexed(s_Data.ModelBuffer2D->GetIndexCount(), 0, 0);
			i++;
		}
	}

	void Renderer::renderLight(const std::shared_ptr<Light>& light)
	{
		static bool inFirst = true;
		if (inFirst)
		{
			s_Data.lightModel = Engine::Model3D::Create()
				.buildFromOBJ()
				.SetObject("Sphere");
			inFirst = false;
		}

		if (light->m_Type == Light::Type::Directional)
			return;

		std::string shadername;
		switch (s_Data.Mode)
		{
		case RenderMode::Forward: shadername = "ForwardLight"; break;
		case RenderMode::Deffered: shadername = "DefferedLight"; break;
		}

		auto shader = ShaderArchive::Get(shadername);
		shader->Bind();

		s_Data.lightModel->m_ModelBuffer->Bind();

		shader->SetParam<CBuffer::Environment>(*s_Data.GlobalEnv);
		shader->SetParam<CBuffer::Transform>(light->lightCam.GetTransform());
		shader->SetParam<CBuffer::Camera>(*s_Data.ActiveCamera);
		shader->SetParam<CBuffer::LightColor>(*light);

		Dx11Core::Get().Context->DrawIndexed(s_Data.lightModel->m_ModelBuffer->GetIndexCount(), 0, 0);
	}

	float Renderer::computeLum(const std::string& target)
	{
		s_Data.LumCsBuffer->SetAsTarget(0);
		
		auto hdr = TextureArchive::Get(target);
		Dx11Core::Get().Context->CSSetShaderResources(0, 1, &hdr->m_ResourceView);

		auto shader = ShaderArchive::Get("HDRAverage");
		auto width = hdr->Width;
		auto height = hdr->Height;
		
		uint32_t dispatchX = (uint32_t)ceil(width / 16.0f);
		uint32_t dispatchY = (uint32_t)ceil(height / 16.0f);
		
		uvec4 vec;
		vec.x = dispatchX;
		vec.y = dispatchY;

		shader->SetParam<CBuffer::DispatchInfo>(vec);
		shader->Bind();
		shader->Dipatch(dispatchX, dispatchY, 1);

		auto data = s_Data.LumCsBuffer->GetData(); //컴퓨트 쉐이더에서 1차처리된 결과를 가져온다.
		float ret = 0.0f;
		for (uint32_t i = 0; i < dispatchY; ++i) //2차 처리는 cpu에서 진행한다.
		{
			for (uint32_t j = 0; j < dispatchX; ++j)
			{
				ret += data[i * dispatchX + j];
			}
		}

		s_Data.LumCsBuffer->UnSetTarget();
		s_Data.LumCsBuffer->Unmap();

		return ret;
	}

	void Renderer::draw2D(std::shared_ptr<Model2D> model, const std::string& shader)
	{
		auto myShader = ShaderArchive::Get("2D");
		myShader->Bind();

		myShader->SetParam<CBuffer::Transform>(model->m_Transform);
		s_Data.ModelBuffer2D->Bind();

		model->m_Texture->Bind(0);

		Dx11Core::Get().Context->PSSetShaderResources(0, 1, &s_Data.SpotShadowMaps[0]->m_ShaderResourceView);

		Dx11Core::Get().Context->DrawIndexed(s_Data.ModelBuffer2D->GetIndexCount(), 0, 0);
	}

	void Renderer::draw3D(std::shared_ptr<Model3D> model, const std::string& shader, int materialBind, bool tess)
	{
		std::string useShader = shader;
		auto meshType = model->m_ModelBuffer->GetMeshType();
		switch (meshType)
		{
		case MeshType::Skeletal: useShader += "Skeletal"; break;
		case MeshType::Static: useShader += "Static"; break;
		case MeshType::SkyBox: useShader += "SkyBox"; break;
		}

		auto myShader = ShaderArchive::Get(useShader);
		myShader->Bind();

		myShader->SetParam<CBuffer::Environment>(*s_Data.GlobalEnv);
		myShader->SetParam<CBuffer::Transform>(model->m_Transform);
		myShader->SetParam<CBuffer::Materials>(*model->m_MaterialSet);
		myShader->SetParam<CBuffer::Bone>(model->m_Animation->MySkinnedTransforms);
		myShader->SetParam<CBuffer::Camera>(*s_Data.ActiveCamera);
		myShader->SetParam<CBuffer::ShadingData>(s_Data.ShadingData_);
		myShader->SetParam<CBuffer::SkyBoxInfo>(s_Data.SkyColor);

		if (shader == "EffectEnvironment")
			myShader->SetParam<CBuffer::CubeCamera>(model->m_Transform.GetTranslate());

		if (meshType == MeshType::SkyBox)
		{
			s_Data.PLController->SetRasterize(RasterlizerOpt::Solid);
			auto skyTexture = model->m_MaterialSet->MaterialTextures[0][0].Name;

			if(!skyTexture.empty())
				TextureArchive::Get(skyTexture)->Bind(0);
		}
		else
		{
			if (myShader->Has(Shader::Type::PixelShader))
				model->m_MaterialSet->BindTextures(materialBind);
		}

		model->m_ModelBuffer->Bind();
		if (tess && meshType != MeshType::SkyBox)
		{
			Dx11Core::Get().Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		}
		

		Dx11Core::Get().Context->DrawIndexed(model->m_ModelBuffer->GetIndexCount(), 0, 0);
		if (meshType == MeshType::SkyBox)
		{
			if (isWire) s_Data.PLController->SetRasterize(RasterlizerOpt::Wire);
			else s_Data.PLController->SetRasterize(RasterlizerOpt::Solid);
		}
		myShader->Unbind();
	}

	void Renderer::renderDeffered()
	{
		//Pass1. Render Geometry Buffer
		s_Data.PLController->SetDepthStencil(DepthStencilOpt::Enable);
		s_Data.PLController->SetBlend(BlendOpt::Alpha);
		if(isWire)
			s_Data.PLController->SetRasterize(RasterlizerOpt::Wire);
		else
			s_Data.PLController->SetRasterize(RasterlizerOpt::Solid);

		std::string useShader = "Deffered";
		s_Data.GeometryBuffer->SetRenderTarget();
		auto skeletal = ShaderArchive::Get(useShader + "Skeletal");
		auto static_ = ShaderArchive::Get(useShader + "Static");
		
		for (auto model3D : s_Data.Queued3D)
			draw3D(model3D, useShader);

		for (auto light : s_Data.QueuedLight)
			renderLight(light);

		//Pass2. Render light with gbuffer
		if (isHdr)
			s_Data.PLController->SetRenderTarget("HDRTexture");
		else
			s_Data.PLController->SetRenderTarget("BackBuffer");

		auto lightingShader = ShaderArchive::Get("DefferedLighting");
		lightingShader->Bind();
		TextureArchive::Get("TempEnvironment")->Bind(10);

		s_Data.ModelBuffer2D->Bind();
		s_Data.GeometryBuffer->Bind();
	
		uint32_t pointIndex = 0;
		uint32_t spotIndex = 0;
		uint32_t dirIndex = 0;

		lightingShader->SetParam<CBuffer::Environment>(*s_Data.GlobalEnv);
		lightingShader->SetParam<CBuffer::Camera>(*s_Data.ActiveCamera);
		lightingShader->SetParam<CBuffer::SkyBoxInfo>(s_Data.SkyColor);

		if (s_Data.QueuedLight.empty())
		{
			Dx11Core::Get().Context->DrawIndexed(s_Data.ModelBuffer2D->GetIndexCount(), 0, 0);
		}
		for (uint32_t i = 0; i < s_Data.QueuedLight.size(); ++i)
		{
			if (i == 0) s_Data.ShadingData_.Data2.y = 0;
			else s_Data.ShadingData_.Data2.y = 1;

			s_Data.PLController->SetDepthStencil(DepthStencilOpt::Disable);
			s_Data.PLController->SetRasterize(RasterlizerOpt::Solid);
			s_Data.PLController->SetBlend(BlendOpt::GBuffer);

			lightingShader->SetParam<CBuffer::ShadingData>(s_Data.ShadingData_);
			lightingShader->SetParam<CBuffer::Light>(*s_Data.QueuedLight[i]);
			lightingShader->SetParam<CBuffer::LightCam>(*s_Data.QueuedLight[i]);

			switch (s_Data.QueuedLight[i]->m_Type)
			{
			case Light::Type::Directional:
				lightingShader->SetParam<CBuffer::Cascaded>(s_Data.QueuedLight[i]->m_CascadedMat);
				Dx11Core::Get().Context->PSSetShaderResources(9, 1, &s_Data.DirShadowMaps[dirIndex]->m_ShaderResourceView);
				dirIndex++;
				break;
			case Light::Type::Point:
				Dx11Core::Get().Context->PSSetShaderResources(8, 1, &s_Data.PointShadowMaps[pointIndex]->m_ShaderResourceView);
				pointIndex++;
				break;
			case Light::Type::Spot:
				Dx11Core::Get().Context->PSSetShaderResources(7, 1, &s_Data.SpotShadowMaps[spotIndex]->m_ShaderResourceView);
				spotIndex++;
				break;
			}
			Dx11Core::Get().Context->DrawIndexed(s_Data.ModelBuffer2D->GetIndexCount(), 0, 0);
		}
		lightingShader->Unbind();

		//processing hdr
		if (isHdr)
		{
			s_Data.PLController->SetRenderTarget("BackBuffer");
			auto averagelum = computeLum("HDRTexture");
			s_Data.ReinhardToneMapFactor[2] = averagelum / float(Dx11Core::Get().Width() * Dx11Core::Get().Height());

			auto hdrShader = ShaderArchive::Get("HDRLighting");
			auto hdrTexture = TextureArchive::Get("HDRTexture");

			hdrShader->Bind();
			hdrTexture->Bind(0);
			hdrShader->SetParam<CBuffer::ToneMapFactor>(s_Data.ReinhardToneMapFactor);
			s_Data.ModelBuffer2D->Bind();

			Dx11Core::Get().Context->DrawIndexed(s_Data.ModelBuffer2D->GetIndexCount(), 0, 0);
		}
	}

	void Renderer::renderForward()
	{
		if (isWire)
			s_Data.PLController->SetRasterize(RasterlizerOpt::Wire);
		else
			s_Data.PLController->SetRasterize(RasterlizerOpt::Solid);

		std::string useShader = "Forward";

		auto skeletal = ShaderArchive::Get(useShader + "Skeletal");
		auto static_ = ShaderArchive::Get(useShader + "Static");
		skeletal->SetParam<CBuffer::TFactor>(tFactor);
		static_->SetParam<CBuffer::TFactor>(tFactor);

		if (!isLighting)
		{
			s_Data.PLController->SetRenderTarget("BackBuffer");
			s_Data.PLController->SetDepthStencil(DepthStencilOpt::Enable);
			s_Data.PLController->SetBlend(BlendOpt::Alpha);

			for (auto& model : s_Data.Queued3D)
				draw3D(model, useShader, 4, true);
		}
		else
		{
			int spotIdx = 0, pointIdx = 0, dirIdx = 0;
			if (s_Data.QueuedLight.empty())
				s_Data.PLController->SetRenderTarget("BackBuffer");
			else
				s_Data.PLController->SetRenderTarget("ForwardTexture");
			
			renderSkyBox();
			for (auto& light : s_Data.QueuedLight)
			{
				s_Data.PLController->SetRenderTarget("ForwardTexture");
				s_Data.PLController->SetDepthStencil(DepthStencilOpt::Enable);
				s_Data.PLController->SetBlend(BlendOpt::Alpha);

				skeletal->SetParam<CBuffer::Light>(*light);
				skeletal->SetParam<CBuffer::LightPos>(*light);
				skeletal->SetParam<CBuffer::LightCam>(*light);
				skeletal->SetParam<CBuffer::Cascaded>(light->m_CascadedMat);
				
				static_->SetParam<CBuffer::Light>(*light);
				static_->SetParam<CBuffer::LightPos>(*light);
				static_->SetParam<CBuffer::LightCam>(*light);
				static_->SetParam<CBuffer::Cascaded>(light->m_CascadedMat);

				switch (light->m_Type)
				{
				case Light::Type::Directional:
					Dx11Core::Get().Context->PSSetShaderResources(2, 1, &s_Data.DirShadowMaps[dirIdx]->m_ShaderResourceView);
					dirIdx++;
					break;
				case Light::Type::Point:
					Dx11Core::Get().Context->PSSetShaderResources(1, 1, &s_Data.PointShadowMaps[pointIdx]->m_ShaderResourceView);
					pointIdx++;
					break;
				case Light::Type::Spot:
					Dx11Core::Get().Context->PSSetShaderResources(0, 1, &s_Data.SpotShadowMaps[spotIdx]->m_ShaderResourceView);
					spotIdx++;
					break;
				}

				int envIdx = 0;
				for (auto& model : s_Data.Queued3D)
				{
					bool isReflect = false;
					auto mat = model->m_MaterialSet->Materials;
					for (auto material : mat)
					{
						if (material.second.Roughness != 1.0f && material.second.Metalic != 0.0f)
						{
							isReflect = true;
							break;
						}
					}
					if (isReflect)
					{
						if (realtimeEnvironment) s_Data.EnvironmentMaps[envIdx++]->Bind(3);
						else TextureArchive::Get("TempEnvironment")->Bind(3);
					}
					draw3D(model, useShader, 4, true);
				}
				renderLight(light);

				s_Data.PLController->SetRenderTarget("BackBuffer");
				s_Data.PLController->SetDepthStencil(DepthStencilOpt::Disable);
				s_Data.PLController->SetRasterize(RasterlizerOpt::Solid);
				s_Data.PLController->SetBlend(BlendOpt::GBuffer);

				auto blendShader = ShaderArchive::Get("ForwardBlend");
				blendShader->Bind();
				TextureArchive::Get("ForwardTexture")->Bind(0);

				s_Data.ModelBuffer2D->Bind();
				Dx11Core::Get().Context->DrawIndexed(s_Data.ModelBuffer2D->GetIndexCount(), 0, 0);
				s_Data.PLController->ClearRTT("ForwardTexture");
			}
		}
	}

	void Renderer::Some()
	{
		s_Data.PLController->SetRasterize(RasterlizerOpt::Solid);
		s_Data.PLController->SetDepthStencil(DepthStencilOpt::Enable);

		s_Data.PLController->SetRenderTarget("BackBuffer");
		
		auto myShader = ShaderArchive::Get("2D");
		myShader->Bind();
		Transform t;
		t.SetScale(0.2f, 0.3f, 1.0f);
		t.SetTranslate(-0.5f, -0.5f, 0.0f);

		myShader->SetParam<CBuffer::Transform>(t);
		s_Data.ModelBuffer2D->Bind();

		Dx11Core::Get().Context->PSSetShaderResources(0, 1, &s_Data.SpotShadowMaps[0]->m_ShaderResourceView);

		Dx11Core::Get().Context->DrawIndexed(s_Data.ModelBuffer2D->GetIndexCount(), 0, 0);
	}
}
