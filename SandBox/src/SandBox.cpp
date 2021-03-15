#include "pceh.h"

#include "SandBox.h"

#include "../../vendor/imgui/imgui.h"

#include "scene/fbxload.h"
#include "scene/3d_animation.h"


float mFactor = 0.0f;
float White = 1.5f;
float Gray = 0.0f;

void SandBox::Init()
{
	{
		std::shared_ptr<Scene> scene;
		scene.reset(new FbxLoadScene);
		scene->Init();
		Scenes.push_back(scene);
		ScenesNames[0] = "Fbx Load";
	}

	{
		std::shared_ptr<Scene> scene;
		scene.reset(new Animation3D);
		scene->Init();
		Scenes.push_back(scene); 
		ScenesNames[1] = "3D Animation play";
	}

	CurScene = Scenes[0];

	switch (renderingPath)
		
	{
	case 0: Engine::Renderer::SetRenderMode(Engine::RenderMode::Forward); break;
	case 1: Engine::Renderer::SetRenderMode(Engine::RenderMode::Deffered); break;
	}
	Engine::Renderer::ActivateHdr(hdr);
	Engine::Renderer::ActivateShadow(shadow);
}

void SandBox::OnImGui()
{
	Engine::ImGuiLayer::Begin();

	//ImGui::ShowDemoWindow();

	ImGui::Begin("General");

	if (ImGui::CollapsingHeader("Overview"))
	{
		auto ms = ts.elapse();
		int fps = int(1.0f / ms);
		ImGui::Text("Fps %d (v-sync)", fps);
		ImGui::Text("Width : %d, Height : %d", g_Width, g_Height);

		if (ImGui::Combo("Current Scene", &curSceneIdx, ScenesNames, 2))
			sceneChanged = true;

		ImGui::BeginChild("cur Camera info", ImVec2(300, 200), true);

		ImGui::Text("Current camera");
		ImGui::Spacing();

		auto& camTransform = CurScene->GetCurCam()->GetTransform();
		auto& camTranslate = camTransform.GetTranslate();
		auto& camRotate = camTransform.GetRotate();

		ImGui::Text("Translate");
		ImGui::Text("x : %f, y : %f, z : %f", camTranslate.x, camTranslate.y, camTranslate.z);
		ImGui::Text("Rotate(degree)");
		ImGui::Text("x : %f, y : %f, z : %f", Engine::Util::ToDegree(camRotate.x), Engine::Util::ToDegree(camRotate.y), Engine::Util::ToDegree(camRotate.z));
		ImGui::EndChild();
	}
	
	if (ImGui::CollapsingHeader("Rendering"))
	{
		static char* rendering[] = { "Forward", "Deffered" };
		if (ImGui::Combo("Rendering path", &renderingPath, rendering, 2))
		{
			pathChanged = true;
			if (renderingPath == 0)
			{
				gBuffer = false;
				Engine::Renderer::ActivateShowGBuffer(gBuffer);
			}
			if (renderingPath == 1)
			{
				pnTesselation = false;
				Engine::Renderer::ActivateTesselation(pnTesselation);
			}
		}

		if (renderingPath == 0)
		{
			if (ImGui::Checkbox("Ligting", &lighting))
			{
				Engine::Renderer::ActivateLighting(lighting);
			}
			
			if (ImGui::Checkbox("Pn Tesselation", &pnTesselation))
			{
				Engine::Renderer::ActivateTesselation(pnTesselation);
			}

			if (pnTesselation)
			{
				if (ImGui::SliderFloat("T-Factor", &tFactor, 1.0f, 20.0f))
					Engine::Renderer::SetTFactor(tFactor);
			}
		}
		if (renderingPath == 1)
		{
			if (ImGui::Checkbox("Show G-Buffer", &gBuffer))
			{
				Engine::Renderer::ActivateShowGBuffer(gBuffer);
			}
			if(ImGui::Checkbox("Shadow", &shadow))
				Engine::Renderer::ActivateShadow(shadow);
			
			if (shadow)
			{
				if(ImGui::SliderFloat("DepthBias", &depthBias, 0.0f, 100.0f))
					Engine::Renderer::AdjustShadowBias(depthBias, slopeBias);
				if (ImGui::SliderFloat("SlopeBias", &slopeBias, 0.0f, 100.0f))
					Engine::Renderer::AdjustShadowBias(depthBias, slopeBias);
			}
			
			if (ImGui::Checkbox("Hdr", &hdr))
			{
				Engine::Renderer::ActivateHdr(hdr);
				if (!hdr)
				{
					Engine::Renderer::ActivateGamma(false);
				}
				else
				{
					Engine::Renderer::ActivateGamma(gamma);
				}
			}
			if (hdr)
			{
				ImGui::BeginChild("", ImVec2(300, 200), true);
				float* factor = Engine::Renderer::GetReinhardFactor();

				ImGui::Text("Average Lum : %f", factor[2]);
				ImGui::SliderFloat("White", &factor[0], 0.0f, 10.0f, nullptr, 1.0f);
				ImGui::SliderFloat("MiddleGray", &factor[1], 0.0f, 10.0f, nullptr, 1.0f);

				ImGui::Spacing();

				if (ImGui::Checkbox("Gamma correction", &gamma))
					Engine::Renderer::ActivateGamma(gamma);

				ImGui::EndChild();
			}
		}
		if (ImGui::Checkbox("Wire", &wire))
		{
			Engine::Renderer::ActivateWire(wire);
		}

		auto glov = Engine::Renderer::GetGlobalEnv();
		
		ImGui::ColorEdit3("Global Ambient", glov->Ambient.m);
	}

	ImGui::End();
	CurScene->OnImGui();
	Engine::ImGuiLayer::End();
}


void SandBox::OnUpdate(float dt)
{
	Engine::Renderer::BeginScene(CurScene->GetCurCam(), CurScene->GetLights());

	CurScene->OnUpdate(dt);
	Engine::Renderer::EndScene();

	OnImGui();
	Engine::Renderer::Present();
	controlUpdate(dt);


	if (sceneChanged)
	{
		sceneChanged = false;
		CurScene = Scenes[curSceneIdx];
	}
	if (pathChanged)
	{
		pathChanged = false;
		if (renderingPath == 0)
		{
			Engine::Renderer::SetRenderMode(Engine::RenderMode::Forward);
		}
		else
		{
			Engine::Renderer::SetRenderMode(Engine::RenderMode::Deffered);
		}
	}

}


//
//void SandBox::OnImGui()
//{
//	Engine::ImGuiLayer::Begin();
//	{
//		ImGui::Begin("check");
//
//		if (ImGui::CollapsingHeader("Light1")){
//			ImGui::ColorEdit4("Color", light->m_Color.m);
//			auto& translate = light->lightCam.GetTransform().GetTranslate();
//			ImGui::SliderFloat("PositionX", &translate.x, -10.0f, 10.0f, nullptr, 1.0f);
//			ImGui::SliderFloat("PositionY", &translate.y, 0, 20.0f, nullptr, 1.0f);
//			ImGui::SliderFloat("PositionZ", &translate.z, -10.0f, 10.0f, nullptr, 1.0f);
//			ImGui::SliderFloat("Intensity", &light->m_Intensity, 0.0f, 10.0f, nullptr, 1.0f);
//		}
//		if (ImGui::CollapsingHeader("Light2")) {
//			ImGui::ColorEdit4("Color", light2->m_Color.m);
//			auto& translate = light2->lightCam.GetTransform().GetTranslate();
//			ImGui::SliderFloat("PositionX", &translate.x, -10.0f, 10.0f, nullptr, 1.0f);
//			ImGui::SliderFloat("PositionY", &translate.y, 0, 20.0f, nullptr, 1.0f);
//			ImGui::SliderFloat("PositionZ", &translate.z, -10.0f, 10.0f, nullptr, 1.0f);
//			ImGui::SliderFloat("Intensity", &light2->m_Intensity, 0.0f, 10.0f, nullptr, 1.0f);
//			ImGui::SliderFloat("InnerAngle", &light2->m_InnerAngle, 0.0f, 1.57f, nullptr, 1.0f);
//			ImGui::SliderFloat("OupterAngle", &light2->m_OuterAngle, 0.0f, 1.57f, nullptr, 1.0f);
//		}
//
//
//		if (ImGui::CollapsingHeader("HDR")) {
//
//			static bool b = true;
//			if (ImGui::Checkbox("On", &b)) Engine::Renderer::ActivateHdr(b);
//
//			if (b)
//			{
//				ImGui::BeginChild("Reinhard");
//				ImGui::SliderFloat("White", &White, 0.0f, 10.0f, nullptr, 1.0f);
//				ImGui::SliderFloat("MiddleGray", &Gray, 0.0f, 10.0f, nullptr, 1.0f);
//				float* factor = Engine::Renderer::GetReinhardFactor();
//				ImGui::Text("Average Lum : %f", factor[2]);
//				ImGui::Text("MiddleGray : %f", factor[1]);
//				ImGui::EndChild();
//			}
//		}
//
//		
//
//		ImGui::End();
//	}
//	
//	Engine::ImGuiLayer::End();
//}
//
//void SandBox::OnAttach()
//{
//	light.reset(new Engine::Light);
//	light->m_Color = { 1.0f, 0.0f, 1.0f, 1.0f };
//	light->lightCam.GetTransform().SetTranslate(1.0f, 2.0f, 0.0f);
//	light->m_Type = Engine::Light::Type::Point;
//	light->m_OuterAngle = 3.141592f / 3;
//	light->m_InnerAngle = 3.141592f / 6;
//	light->m_Range = 20.0f;
//	light->lightCam.GetTransform().SetRotate(0.78f, -1.57f, 0.0f);
//
//	light2.reset(new Engine::Light);
//	light2->m_Color = { 0.0, 0.0f, 1.0f, 1.0f };
//	light2->lightCam.GetTransform().SetTranslate(-1.0f, 2.0f, 0.0f);
//	light2->m_Type = Engine::Light::Type::Spot;
//	light2->m_OuterAngle = 3.141592f / 3;
//	light2->m_InnerAngle = 3.141592f / 6;
//	light2->m_Range = 30.0f;
//	light2->lightCam.GetTransform().SetRotate(0.78f, 1.57f, 0.0f);
//
//	fbxmodel = Engine::Model3D::Create()
//		.buildFromFBX().SetSkeleton("Pearl");
//	fbxmodel->m_Transform.SetScale(0.01f, 0.01f, 0.01f);
//
//	for (auto& mat : fbxmodel->m_MaterialSet->Materials)
//	{
//		mat.second.Ambient.x = 0.8f;
//		mat.second.Ambient.y = 0.8f;
//		mat.second.Ambient.z = 0.8f;
//
//		mat.second.Specular.x = 0.5f;
//		mat.second.Specular.y = 0.5f;
//		mat.second.Specular.z = 0.5f;
//	}
//
//	objmodel = Engine::Model3D::Create()
//		.buildFromOBJ().SetObject("Triangle");
//
//	//auto& texture = objmodel->m_MaterialSet->MaterialTextures[0];
//	//texture[0].Name = "skyBox";
//	//objmodel->m_MaterialSet->Materials[0].MapMode |= 1;
//	objmodel->m_Transform.SetScale(10.0f, 10.0f, 10.0f);
//	//objmodel->m_Transform.SetTranslate(0.0f, 10.0f, 0.0f);
//
//	for (auto& mat : objmodel->m_MaterialSet->Materials)
//	{
//		mat.second.Ambient.x = 1.0f;
//		mat.second.Ambient.y = 1.0f;
//		mat.second.Ambient.z = 1.0f;
//	}
//
//	floor = Engine::Model3D::Create()
//		.buildFromOBJ().SetObject("RoomCube_t1");
//
//	floor->m_Transform.SetScale(10.0f, 10.0f, 10.0f);
//	floor->m_Transform.SetTranslate(0.0f, 10.0f, 0.0f);
//
//	debugwindow = Engine::Model2D::Create()
//		.SetTexture("images");
//	debugwindow->m_Transform.SetTranslate(0.5f, 0.5f, 0.0f);
//	debugwindow->m_Transform.SetScale(0.2f, 0.3f, 0.0f);
//
//	float filedOfView = 3.141592f / 3.0f;
//	perspective.reset(new Engine::Camera(filedOfView, float(width) / (float)height));
//	perspective->GetTransform().SetTranslate(0.0f, 2.0f, -4.0f);
//
//	/*Engine::Model3D::Create().
//		buildCustum().SetMesh("Triangle");*/
//
//}
//
//void SandBox::OnDettach()
//{
//}
//
void SandBox::OnResize()
{
	auto& cam = CurScene->GetCurCam();
	cam->OnResize(g_Width, g_Height);
}
//
void SandBox::OnMouseMove(float dx, float dy)
{
	auto& camTransform = CurScene->GetCurCam()->GetTransform();
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		dx *= mouseSensitive;
		dy *= mouseSensitive;
		camTransform.LocalRotateX(dx);
		camTransform.LocalRotateY(dy);
	}
}

void SandBox::controlUpdate(float dt)
{
	auto& camTransform = CurScene->GetCurCam()->GetTransform();

	if (GetAsyncKeyState('W') & 0x8000)
	{
		camTransform.MoveForwad(0.1f);
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		camTransform.MoveBack(0.1f);
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		camTransform.MoveLeft(0.1f);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		camTransform.MoveRight(0.1f);
	}

	if (GetAsyncKeyState('Q') & 0x8000)
	{
		camTransform.AddTranslate(0.0f, 0.1f, 0.0f);
	}
	if (GetAsyncKeyState('E') & 0x8000)
	{
		camTransform.AddTranslate(0.0f, -0.1f, 0.0f);
	}
}

//void ShadowBiasControl()
//{
//
//	static float sensitive = 0.0001f;
//	static int ss = 1;
//
//	if (GetAsyncKeyState(VK_F1) & 0x8000)
//	{
//		sensitive *= 10.0f;
//		std::cout << "bias sensitive" << sensitive << '\n';
//	}
//	if (GetAsyncKeyState(VK_F2) & 0x8000)
//	{
//		sensitive *= 0.1f;
//		std::cout << "bias sensitive" << sensitive << '\n';
//	}
//	if (GetAsyncKeyState(VK_F3) & 0x8000)
//	{
//		sensitive += sensitive;
//		std::cout << "bias sensitive" << sensitive << '\n';
//	}
//	if (GetAsyncKeyState(VK_F4) & 0x8000)
//	{
//		sensitive -= sensitive;
//		std::cout << "bias sensitive" << sensitive << '\n';
//	}
//
//	if (GetAsyncKeyState(VK_F5) & 0x8000)
//	{
//		Engine::Renderer::AdjustDepthBias(ss);
//	}
//	if (GetAsyncKeyState(VK_F6) & 0x8000)
//	{
//		Engine::Renderer::AdjustDepthBias(-ss);
//	}
//	if (GetAsyncKeyState(VK_F7) & 0x8000)
//	{
//		Engine::Renderer::AdjustSlopeBias(sensitive);
//	}
//	if (GetAsyncKeyState(VK_F8) & 0x8000)
//	{
//		Engine::Renderer::AdjustSlopeBias(-sensitive);
//	}
//}