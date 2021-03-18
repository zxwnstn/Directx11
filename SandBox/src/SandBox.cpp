#include "pceh.h"

#include "SandBox.h"

#include "../../vendor/imgui/imgui.h"


float mFactor = 0.0f;
float White = 1.5f;
float Gray = 0.0f;

void SandBox::Init()
{
	std::shared_ptr<Scene> scene(new Scene("Scene1")); 
	{
		////Model
		std::shared_ptr<Engine::Model3D> model = Engine::Model3D::Create()
			.buildFromFBX()
			.SetSkeleton("Kachujin");

		model->m_Name = "Kachujin";
		model->m_Transform.SetScale(0.01f, 0.01f, 0.01f);

		std::shared_ptr<Engine::Model3D> floor = Engine::Model3D::Create()
			.buildFromOBJ()
			.SetObject("Square");

		floor->m_Name = "floor";
		floor->m_Transform.SetScale(5.0f, 5.0f, 1.0f);
		floor->m_Transform.SetRotate(1.572f, 0.0f, 0.0f);

		scene->Add3DModel(model);
		scene->Add3DModel(floor);
	}
	Scenes.push_back(scene);

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

	ImGui::Begin("General");

	if (ImGui::CollapsingHeader("Overview"))
	{
		auto ms = ts.elapse();
		int fps = int(1.0f / ms);
		ImGui::Text("Fps %d (v-sync)", fps);
		ImGui::Text("Width : %d, Height : %d", g_Width, g_Height);

		ImGui::BeginChild("cur Camera info", ImVec2(300, 200), true);

		ImGui::Text("Camera info");
		ImGui::Spacing();

		auto& curCam = CurScene->GetCurCam();
		auto& camTransform = curCam->GetTransform();
		auto& camTranslate = camTransform.GetTranslate();
		auto& camRotate = camTransform.GetRotate();

		const char* type;
		int camtype = curCam->GetCamType();
		switch (camtype)
		{
		case 0: type = "Otho"; break;
		case 1: type = "Perspective"; break;
		}

		ImGui::Text("Camera name : %s", curCam->GetName().c_str());
		ImGui::Text("Camera Tpye : %s", type);
		if (camtype == 0)
			ImGui::Text("Magnification : %f", curCam->GetMag());
		else
			ImGui::Text("Fov : %f", curCam->GetFov());
		ImGui::Text("near : %f", curCam->GetNear());
		ImGui::Text("far: %f", curCam->GetFar());

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
		}

		auto glov = Engine::Renderer::GetGlobalEnv();
		ImGui::ColorEdit3("Global Ambient", glov->Ambient.m);
		ImGui::ColorEdit3("Sky Color", Engine::Renderer::GetSkyColor().m);

		if (ImGui::Checkbox("WireFrame", &wire))
			Engine::Renderer::ActivateWire(wire);

		if (ImGui::Checkbox("Shadow", &shadow))
			Engine::Renderer::ActivateShadow(shadow);

		if (shadow)
		{
			if (ImGui::SliderFloat("DepthBias", &depthBias, 0.0f, 100.0f))
				Engine::Renderer::AdjustShadowBias(depthBias, slopeBias);
			if (ImGui::SliderFloat("SlopeBias", &slopeBias, 0.0f, 100.0f))
				Engine::Renderer::AdjustShadowBias(depthBias, slopeBias);
		}

		if (renderingPath == 0)
		{
			if (ImGui::Checkbox("PhongShading", &lighting))
			{
				Engine::Renderer::ActivateLighting(lighting);
			}

			if (ImGui::SliderFloat("TsellationFactor", &tFactor, 1.0f, 20.0f))
				Engine::Renderer::SetTFactor(tFactor);
		}
		if (renderingPath == 1)
		{
			if (ImGui::Checkbox("Show G-Buffer", &gBuffer))
			{
				Engine::Renderer::ActivateShowGBuffer(gBuffer);
			}

			if (ImGui::Checkbox("Gamma correction", &gamma))
				Engine::Renderer::ActivateGamma(gamma);

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

				ImGui::EndChild();
			}
		}		
	}

	if (ImGui::CollapsingHeader("Scene"))
	{
		const char* ScenesNames[100];
		int i = 0;
		for (int i = 0; i < Scenes.size(); ++i)
			ScenesNames[i] = Scenes[i]->GetSceneName().c_str();

		if (ImGui::Combo("", &curSceneIdx, ScenesNames, Scenes.size()))
			sceneChanged = true;

		ImGui::SameLine();
		if (ImGui::Button("Re-name"))
			newName = true;

		if (ImGui::Button("New", ImVec2(55.0f, 20.0f)))
			newScene = true;

		ImGui::SameLine();
		if (ImGui::Button("Delete", ImVec2(55.0f, 20.0f)))
		{
			if (Scenes.size() != 1)
			{
				Scenes.erase(Scenes.begin() + curSceneIdx);

				if (curSceneIdx == Scenes.size())
				{
					--curSceneIdx;
				}

				CurScene = Scenes[curSceneIdx];
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Save", ImVec2(55.0f, 20.0f)))
		{
			if (!onSave)
			{
				onSave = true;
				Engine::File::OpenSaveFileDialog(Engine::File::GetCommonPath(Engine::File::CommonPathType::Scene));
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Load", ImVec2(55.0f, 20.0f)))
		{
			if (!onLoad)
			{
				onLoad = true;
				Engine::File::OpenReadFileDialog(Engine::File::GetCommonPath(Engine::File::CommonPathType::Scene));
			}
		}
	}

	ImGui::End();

	if (newScene)
	{
		ImGui::Begin("Create Scene");
		ImGui::InputText("sceneName", newSceneBuffer, 100);
		if (ImGui::Button("Ok"))
		{
			std::shared_ptr<Scene> scene(new Scene(newSceneBuffer));
			Scenes.push_back(scene);
			CurScene = scene;
			curSceneIdx = Scenes.size() - 1;
			memset(newSceneBuffer, 0, 100);
			newScene = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			memset(newSceneBuffer, 0, 100);
			newScene = false;
		}
		ImGui::End();
	}
	if (newName)
	{
		ImGui::Begin("Create Scene");
		ImGui::InputText("sceneName", newSceneBuffer, 100);
		if (ImGui::Button("Ok"))
		{
			CurScene->SetSceneName(newSceneBuffer);
			memset(newSceneBuffer, 0, 100);
			newName = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			memset(newSceneBuffer, 0, 100);
			newName = false;
		}
		ImGui::End();
	}
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

	if (onSave || onLoad)
	{
		auto path = Engine::File::GetDialogResult();
		if (!path.empty())
		{
			if (onSave)
			{
				auto extension = path.find(".scene");
				if (extension == std::string::npos)
					path += ".scene";
				SaveScene(path);
			}
			if (onLoad)
				LoadScene(path);


			onSave = false;
			onLoad = false;
		}
	}


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

void SandBox::OnResize()
{
	auto& cams = CurScene->GetCams();
	for (auto cam : cams)
	{
		cam->OnResize(g_Width, g_Height);
	}
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

void SandBox::SaveScene(const std::string& path)
{
	auto slash = path.rfind('\\');
	auto extension = path.rfind('.');
	auto saveSceneName = path.substr(slash + 1, extension - slash - 1);
	auto inform = CurScene->Save();
	inform.SceneName = saveSceneName;

	Engine::Serializer::Write(path, inform);

	LOG_INFO("{0} Scene saved complete!(Path : {1})", saveSceneName, path);
}

void SandBox::LoadScene(const std::string& path)
{
	SceneInform inform;
	Engine::Serializer::Read(path, inform);
	std::shared_ptr<Scene> scene(new Scene(inform));
	Scenes.push_back(scene);

	CurScene = scene;
	curSceneIdx = Scenes.size() - 1;

	LOG_INFO("{0} Load scene complete!(Path : {1})", inform.SceneName, path);
}

