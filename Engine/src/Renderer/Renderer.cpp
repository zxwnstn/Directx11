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


		RenderMode Mode;
		RenderingPath Path;

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

		s_Data.Mode = RenderMode::Forward;

		for (uint32_t i = 0; i < MAX_LIGHT; ++i)
			s_Data.SpotShadowMaps.emplace_back(new ShadowMap(2048, 2048));
		for (uint32_t i = 0; i < 10; ++i)
			s_Data.PointShadowMaps.emplace_back(new ShadowMap(4096, 4096, 6));

		TextureArchive::Add("HDRTexture", prop.Width, prop.Height);
		TextureArchive::Add("ForwardTexture", prop.Width, prop.Height);

		s_Data.LumCsBuffer.reset(new SBuffer<float>(SBufferType::Write, nullptr, 20000));

		s_Data.emptyLight.reset(new Light);
		s_Data.emptyLight->noLight = true;

		s_Data.ReinhardToneMapFactor[0] = 0.1f;
	}

	void Renderer::Shutdown()
	{
	}

	void Renderer::BeginScene(std::shared_ptr<Camera> camera, const std::vector<std::shared_ptr<Light>>& lights)
	{
		s_Data.PLController->ClearRTT();
		s_Data.ActiveCamera = camera;
		s_Data.QueuedLight = lights;
	}

	void Renderer::Enque2D(std::shared_ptr<Model2D> model)
	{
		s_Data.Queued2D.push_back(model);
	}

	void Renderer::Enque3D(std::shared_ptr<Model3D> model)
	{
		s_Data.Queued3D.push_back(model);
	}

	bool isHdr = false;
	bool isGamma = false;
	bool isAutoMiddelGray = false;

	void Renderer::ActivateHdr(bool activate)
	{
		isHdr = activate;
	}

	void Renderer::ActivateShadow(bool activate)
	{
		s_Data.GlobalEnv->UseShadowMap = activate;
	}

	void Renderer::ActivateGamma(bool activate)
	{
		isGamma = activate;
	}

	void Renderer::ActivateAutoMiddleGray(bool activate)
	{
		isAutoMiddelGray = activate;
	}

	void Renderer::EndScene()
	{
		for (auto&[name, shader] : ShaderArchive::s_Shaders)
			shader->SetParam<CBuffer::Environment>(*s_Data.GlobalEnv);

		renderShadow();

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

		s_Data.Queued2D.clear();
		s_Data.Queued3D.clear();
		s_Data.QueuedLight.clear();
	}

	void Renderer::Present()
	{
		Dx11Core::Get().Present();
	}

	void Renderer::excompute()
	{
		unsigned int arr[126];
		for (int i = 0; i < 126; ++i)
			arr[i] = i;

		unsigned int arr2[126]{ 0 };

		SBuffer<unsigned int> readBuffer(SBufferType::Read, arr, 126);
		SBuffer<unsigned int> writeBuffer(SBufferType::Write, arr2, 126);

		auto shader = ShaderArchive::Get("ComputeBasic");
		shader->Bind();

		readBuffer.Bind(0);
		writeBuffer.SetAsTarget(0);
		shader->Dipatch(2, 3, 1);

		auto v = writeBuffer.GetData();
		for (int i = 0; i < 130; ++i)
		{
			std::cout << v[i] << " ";
			if (i % 10 == 0) std::cout << "\n";
		}
	}

	void Renderer::excompute2(std::shared_ptr<Model3D> model)
	{
		static bool first = true;
		static std::shared_ptr<Texture> uav;
		static std::shared_ptr<Texture> stone = TextureArchive::Get("stone01");
		static std::shared_ptr<Texture> apple = TextureArchive::Get("apple");
		if (first)
		{
			TextureArchive::Add("UAV", 512, 512, true, true);
			auto shader = ShaderArchive::Get("ComputeTexture");
			shader->Bind();

			uav = TextureArchive::Get("UAV");
			stone->SetComputeResource(0);
			apple->SetComputeResource(1);
			uav->SetComputeOuput();

			shader->Dipatch(32, 32, 1);
			first = false;
			shader->Unbind();

			//Notice : This Process must be needed!(Unbind Resource)
			ID3D11UnorderedAccessView* nullUAV = nullptr;
			Dx11Core::Get().Context->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
		}

		auto twod = ShaderArchive::Get("2D");
		twod->Bind();

		s_Data.PLController->ClearRTT();

		s_Data.PLController->SetRenderTarget("BackBuffer");

		s_Data.PLController->SetDepthStencil(DepthStencilOpt::Enable);
		s_Data.PLController->SetRasterize(RasterlizerOpt::Solid);

		Engine::Transform t;
		t.SetScale(0.5f, 0.5f, 1.0f);

		twod->SetParam<CBuffer::Transform>(t);

		s_Data.ModelBuffer2D->Bind();
		uav->Bind(0);
		Dx11Core::Get().Context->DrawIndexed(s_Data.ModelBuffer2D->GetIndexCount(), 0, 0);
	}

	void Renderer::exstreamout(std::shared_ptr<Model3D> model)
	{
		s_Data.PLController->ClearRTT();

		s_Data.PLController->SetRenderTarget("BackBuffer");

		s_Data.PLController->SetDepthStencil(DepthStencilOpt::Disable);
		s_Data.PLController->SetRasterize(RasterlizerOpt::Wire);

		auto shader = ShaderArchive::Get("EtcStreamout");
		shader->Bind();
		ID3D11PixelShader* ps = nullptr;

		SOBuffer sobuffer;
		sobuffer.Bind();

		model->m_ModelBuffer->Bind();
		Dx11Core::Get().Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

		Dx11Core::Get().Context->DrawIndexed(model->m_ModelBuffer->GetIndexCount(), 0, 0);
		sobuffer.Unbind();
		shader->Unbind();

		auto d = sobuffer.GetData();
		for (int i = 0; i < 100; ++i)
		{
			//d[i].print();
		}

		shader = ShaderArchive::Get("EtcCheckSO");
		shader->Bind();

		uint32_t stride = 36;
		uint32_t offset = 0;
		Dx11Core::Get().Context->IASetVertexBuffers(0, 1, &sobuffer.m_OutstreamBuffer, &stride, &offset);
		Dx11Core::Get().Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Dx11Core::Get().Context->DrawAuto();
	}

	void Renderer::exavlum(const std::string& texture)
	{
		auto tex = TextureArchive::Get(texture);

		auto width = tex->Width;
		auto height = tex->Height;

		{
			ENABLE_ELAPSE
				auto shader = ShaderArchive::Get("HDRAverage");

			shader->Bind();
			float* a = nullptr;
			SBuffer<float> buffer(SBufferType::Write, a, 20000);
			buffer.SetAsTarget(0);
			Dx11Core::Get().Context->CSSetShaderResources(0, 1, &tex->m_ResourceView);

			uint32_t dispatchX = (uint32_t)ceil(width / 16.0f);
			uint32_t dispatchY = (uint32_t)ceil(height / 16.0f);

			uvec4 vec;
			vec.x = dispatchX;
			vec.y = dispatchY;
			shader->SetParam<CBuffer::DispatchInfo>(vec);

			shader->Dipatch(dispatchX, dispatchY, 1);
			buffer.UnSetTarget();

			auto data = buffer.GetData();
			float averageLum = 0.0f;
			for (uint32_t i = 0; i < dispatchY; ++i)
			{
				for (uint32_t j = 0; j < dispatchX; ++j)
				{
					averageLum += data[i * dispatchX + j];
				}
			}
			averageLum /= width * height;
			buffer.Unmap();
			LOG_ELAPSE
				LOG_INFO("{0} gpu average", averageLum);
		}
	}

	void Renderer::exhdr()
	{
		uint32_t dispatchX = (uint32_t)ceil(Dx11Core::Get().Width() / 16.0f);
		uint32_t dispatchY = (uint32_t)ceil(Dx11Core::Get().Height() / 16.0f);
		auto shader = ShaderArchive::Get("HDRAverage");

		shader->Bind();
		SBuffer<float> buffer(SBufferType::Write, nullptr, 1);
		buffer.Bind(0);
		//texture.Bind(0); // HDR texture

		shader->Dipatch(dispatchX, dispatchY, 1);
		buffer.UnSetTarget();

		auto data = buffer.GetData();
		float averageLum = 0.0f;

		for (uint32_t i = 0; i < dispatchY; ++i)
		{
			for (uint32_t j = 0; j < dispatchX; ++j)
			{
				averageLum += data[i * dispatchX + j];
			}
		}
		averageLum /= float(Dx11Core::Get().Width() *Dx11Core::Get().Height());
	}

	void Renderer::experiment1(std::shared_ptr<Model3D> model, float tFactor)
	{
		s_Data.PLController->ClearRTT();

		s_Data.PLController->SetRenderTarget("BackBuffer");

		s_Data.PLController->SetDepthStencil(DepthStencilOpt::Enable);
		s_Data.PLController->SetRasterize(RasterlizerOpt::Wire);

		//auto d2 = ShaderArchive::Get("2D");
		//d2->Bind();

		//Transform t;
		//t.SetScale(0.3f, 0.3f, 0.3f);
		//d2->SetParam<CBuffer::Transform>(t);
		//s_Data.ModelBuffer2D->Bind();
		////model->m_Texture->Bind(0);
		////Dx11Core::Get().Context->PSSetShaderResources(0, 1, &s_Data.SpotShadowMaps[0]->m_ShaderResourceView);

		//Dx11Core::Get().Context->DrawIndexed(s_Data.ModelBuffer2D->GetIndexCount(), 0, 0);
		//d2->Unbind();

		auto myShader = ShaderArchive::Get("EtcPNTriangle");
		//auto myShader = ShaderArchive::Get("Etcbasic");
		myShader->Bind();
		myShader->SetParam<CBuffer::Transform>(model->m_Transform);
		myShader->SetParam<CBuffer::Camera>(*s_Data.ActiveCamera);
		myShader->SetParam<CBuffer::TFactor>(tFactor);

		model->m_ModelBuffer->Bind();
		Dx11Core::Get().Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		Dx11Core::Get().Context->DrawIndexed(model->m_ModelBuffer->GetIndexCount(), 0, 0);

		s_Data.Queued2D.clear();
		s_Data.Queued3D.clear();
		s_Data.QueuedLight.clear();

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

		auto shader = ShaderArchive::Get("EtcDrawLight");
		shader->Bind();

		s_Data.lightModel->m_ModelBuffer->Bind();

		light->lightCam.GetTransform().SetScale(0.2f, 0.2f, 0.2f);
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

		auto data = s_Data.LumCsBuffer->GetData();
		float ret = 0.0f;
		for (uint32_t i = 0; i < dispatchY; ++i)
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


	float* Renderer::GetReinhardFactor()
	{
		return s_Data.ReinhardToneMapFactor;
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

	void Renderer::draw3D(std::shared_ptr<Model3D> model, const std::string& shader, int materialBind)
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

		if (myShader->Has(Shader::Type::PixelShader))
			model->m_MaterialSet->BindTextures(materialBind);

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

		TextureArchive::Get("ForwardTexture")->Resize(width, height);
		TextureArchive::Get("HDRTexture")->Resize(width, height);
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

	void Renderer::renderDeffered()
	{
		//Pass1. Render Geometry Buffer
		s_Data.GeometryBuffer->SetRenderTarget();

		s_Data.PLController->SetDepthStencil(DepthStencilOpt::Enable);
		s_Data.PLController->SetRasterize(RasterlizerOpt::Solid);
		s_Data.PLController->SetBlend(BlendOpt::Alpha);

		std::string useShader = "Deffered";
		auto skeletal = ShaderArchive::Get(useShader + "Skeletal");
		skeletal->Bind();
		skeletal->SetParam<CBuffer::Camera>(*s_Data.ActiveCamera);

		auto static_ = ShaderArchive::Get(useShader + "Static");
		static_->Bind();
		static_->SetParam<CBuffer::Camera>(*s_Data.ActiveCamera);

		auto lighting = ShaderArchive::Get(useShader + "Lighting");
		lighting->Bind();
		lighting->SetParam<CBuffer::Camera>(*s_Data.ActiveCamera);

		std::string shader;
		switch (s_Data.Mode)
		{
		case RenderMode::Deffered: shader = "Deffered"; break;
		case RenderMode::Forward: shader = "Forward"; break;
		}
		for (auto model3D : s_Data.Queued3D)
			draw3D(model3D, shader);

		//Pass2. Render light with gbuffer
		if (isHdr)
			s_Data.PLController->SetRenderTarget("HDRTexture");
		else
			s_Data.PLController->SetRenderTarget("BackBuffer");

		auto lightingShader = ShaderArchive::Get("DefferedLighting");
		lightingShader->Bind();
		s_Data.ModelBuffer2D->Bind();
		s_Data.GeometryBuffer->Bind();


		uvec4 gamma;
		if (isGamma)
			gamma.x = 1;
		else
			gamma.x = 0;
		lightingShader->SetParam<CBuffer::Gamma>(gamma);
		
		uint32_t pointIndex = 0;
		uint32_t spotIndex = 0;

		for (uint32_t i = 0; i < s_Data.QueuedLight.size(); ++i)
		{
			s_Data.PLController->SetDepthStencil(DepthStencilOpt::Disable);
			s_Data.PLController->SetBlend(BlendOpt::GBuffer);

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
			s_Data.PLController->SetDepthStencil(DepthStencilOpt::Enable);	
		}
		lightingShader->Unbind();

		if (isHdr)
		{
			s_Data.PLController->SetRenderTarget("BackBuffer");
			auto averagelum = computeLum("HDRTexture");

			auto hdrShader = ShaderArchive::Get("HDRLighting");
			auto hdrTexture = TextureArchive::Get("HDRTexture");
			hdrShader->Bind();

			s_Data.ReinhardToneMapFactor[2] = averagelum / float(Dx11Core::Get().Width() * Dx11Core::Get().Height());
			if (isAutoMiddelGray)
				s_Data.ReinhardToneMapFactor[1] = 1.03f - 2.0f / (2.0f + log10(s_Data.ReinhardToneMapFactor[2] + 1));

			hdrShader->SetParam<CBuffer::ToneMapFactor>(s_Data.ReinhardToneMapFactor);
			hdrTexture->Bind(0);
			
			s_Data.ModelBuffer2D->Bind();
			Dx11Core::Get().Context->DrawIndexed(s_Data.ModelBuffer2D->GetIndexCount(), 0, 0);
		}
	}

	void Renderer::renderForward()
	{
		std::string useShader = "Forward";

		auto skeletal = ShaderArchive::Get(useShader + "Skeletal");
		skeletal->Bind();
		skeletal->SetParam<CBuffer::Camera>(*s_Data.ActiveCamera);

		auto static_ = ShaderArchive::Get(useShader + "Static");
		static_->Bind();
		static_->SetParam<CBuffer::Camera>(*s_Data.ActiveCamera);

		if (s_Data.QueuedLight.empty())
		{
			s_Data.PLController->SetRenderTarget("BackBuffer");

			s_Data.PLController->SetDepthStencil(DepthStencilOpt::Enable);
			s_Data.PLController->SetRasterize(RasterlizerOpt::Solid);
			s_Data.PLController->SetBlend(BlendOpt::Alpha);

			skeletal->SetParam<CBuffer::Light>(*s_Data.emptyLight);

			static_->SetParam<CBuffer::Light>(*s_Data.emptyLight);

			for (auto& model : s_Data.Queued3D)
				draw3D(model, useShader, 1);
		}
		else
		{
			int type = -1;
			int spotIdx = -1, pointIdx = -1, dirIdx = -1, curIdx = -1;

			for (auto& light : s_Data.QueuedLight)
			{
				s_Data.PLController->ClearRTT("ForwardTexture");

				s_Data.PLController->SetRenderTarget("ForwardTexture");
				s_Data.PLController->SetDepthStencil(DepthStencilOpt::Enable);
				s_Data.PLController->SetRasterize(RasterlizerOpt::Solid);
				s_Data.PLController->SetBlend(BlendOpt::Alpha);

				skeletal->SetParam<CBuffer::Light>(*light);
				skeletal->SetParam<CBuffer::LightPos>(*light);
				static_->SetParam<CBuffer::Light>(*light);
				static_->SetParam<CBuffer::LightPos>(*light);

				switch (light->m_Type)
				{
				case Light::Type::Directional: type = 0;  curIdx = ++dirIdx; break;
				case Light::Type::Point: type = 1; curIdx = ++pointIdx; break;
				case Light::Type::Spot: type = 2; curIdx = ++spotIdx; break;
				}

				for (auto& model : s_Data.Queued3D)
					draw3D(model, useShader, 1);

				renderLight(light);

				s_Data.PLController->SetRenderTarget("BackBuffer");
				s_Data.PLController->SetDepthStencil(DepthStencilOpt::Disable);
				s_Data.PLController->SetBlend(BlendOpt::GBuffer);

				auto blendShader = ShaderArchive::Get("ForwardBlend");

				blendShader->Bind();
				TextureArchive::Get("ForwardTexture")->Bind(0);

				s_Data.ModelBuffer2D->Bind();
				Dx11Core::Get().Context->DrawIndexed(s_Data.ModelBuffer2D->GetIndexCount(), 0, 0);
			}
		}
	}

	void Renderer::Some()
	{
	}
}
