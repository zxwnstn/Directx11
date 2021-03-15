#include "pceh.h"

#include "fbxload.h"
#include "../../vendor/imgui/imgui.h"

const char* arr[10];
const char* arr2[10];

void FbxLoadScene::Init()
{
	m_Name = "One model scene";

	//Cam
	float filedOfView = 3.141592f / 3.0f;
	m_Cur_cam.reset(new Engine::Camera(filedOfView, float(g_Width) / (float)g_Height));
	m_Cur_cam->GetTransform().SetTranslate(0.0f, 2.0f, 4.0f);
	m_Cur_cam->GetTransform().SetRotate(0.0f, 3.14f, 0.0f);

	m_Cameras.push_back(m_Cur_cam);

	std::shared_ptr<Engine::Light> light2(new Engine::Light);
	m_Lights.push_back(light2);
	light2->m_Type = Engine::Light::Type::Point;
	light2->lightCam.GetTransform().SetTranslate(0.0f, 5.0f, 5.0f);
	light2->name = "Light1";


	//Model
	ModelNames = Engine::SkeletonArchive::GetAllName();

	for (int i = 0; i < ModelNames.size(); ++i)
	{
		std::shared_ptr<Engine::Model3D> model = Engine::Model3D::Create()
			.buildFromFBX()
			.SetSkeleton(ModelNames[i].c_str());
		model->m_Transform.SetScale(0.01f, 0.01f, 0.01f);
		m_Model3.push_back(model);
		curAnimtionIdx[ModelNames[i]] = 0;
	}

	floor = Engine::Model3D::Create()
		.buildFromOBJ()
		.SetObject("Square");

	floor->m_Transform.SetScale(5.0f, 5.0f, 1.0f);
	floor->m_Transform.SetRotate(1.572f, 0.0f, 0.0f);

	std::shared_ptr<Engine::Model2D> fix2d = Engine::Model2D::Create().SetTexture("");
	fix2d->m_Transform.SetScale(0.2f, 0.3f, 1.0f);
	fix2d->m_Transform.SetTranslate(-0.7f, -0.5f, 1.0f);
	m_Model2.push_back(fix2d);

	for (int i = 0; i < ModelNames.size(); ++i)
		arr[i] = ModelNames[i].c_str();
}

void FbxLoadScene::OnUpdate(float dt)
{
	if (addLight)
	{
		addLight = false;
		std::shared_ptr<Engine::Light> light(new Engine::Light);
		light->m_Type = Engine::Light::Type::Point;
		light->lightCam.GetTransform().SetTranslate(0.0f, 5.0f, 5.0f);
		light->name = "new light" + std::to_string(++newlightCnt);
		m_Lights.push_back(light);
	}
	if (deleteLight)
	{
		deleteLight = false;
		m_Lights.erase(m_Lights.begin() + selectedLight);
		if (selectedLight == m_Lights.size())
			--selectedLight;
	}

	m_Model3[curModelIdx]->Update(animationSpeed);

	Engine::Renderer::Enque3D(m_Model3[curModelIdx]);
	Engine::Renderer::Enque3D(floor);

	for (auto model : m_Model2); //Engine::Renderer::Enque2D(model);
}

void FbxLoadScene::OnImGui()
{
	auto animlist = Engine::SkeletalAnimationArchive::GetAnimList(ModelNames[curModelIdx]);

	arr2[0] = "T-Pose";
	for (int i = 0; i < animlist.size(); ++i)
		arr2[i + 1] = animlist[i].c_str();

	ImGui::Begin("Current Scene");

	ImGui::Combo("Model", &curModelIdx, arr, ModelNames.size());

	if (ImGui::CollapsingHeader("Animation"))
	{
		ImGui::BeginChild("AnimationTab", ImVec2(350, 100), true);
		if (ImGui::Combo("motion", &curAnimtionIdx[ModelNames[curModelIdx]], arr2, animlist.size() + 1))
		{
			if (curAnimtionIdx[ModelNames[curModelIdx]] == 0)
			{
				m_Model3[curModelIdx]->StopAnimation();
			}
			else
			{
				m_Model3[curModelIdx]->PlayAnimation();
				m_Model3[curModelIdx]->SetAnimation(animlist[curAnimtionIdx[ModelNames[curModelIdx]] - 1], true);
			}
		}

		if (curAnimtionIdx[ModelNames[curModelIdx]] != 0)
		{
			ImGui::SliderFloat("Play Speed (ms)", &animationSpeed, 0.0f, 0.1f);
		}
		ImGui::EndChild();
	}

	if (ImGui::CollapsingHeader("Model"))
	{
		ImGui::BeginChild("TRSTab", ImVec2(350, 100), true);
		auto& transform = m_Model3[curModelIdx]->m_Transform;

		auto& translate = transform.GetTranslate();
		auto& rotate = transform.GetRotate();
		auto& scale = transform.GetScale();
		float trans[3] = { translate.x, translate.y, translate.z };
		float rot[3] = { Engine::Util::ToDegree(rotate.x), Engine::Util::ToDegree(rotate.y), Engine::Util::ToDegree(rotate.z )};
		float sca[3] = { scale.x * 100, scale.y * 100, scale.z * 100 };

		ImGui::Text("Transform");
		ImGui::InputFloat3("Translate", translate.m);
		if (ImGui::InputFloat3("Rotate(radian)", rot))
			transform.SetRotate(Engine::Util::ToRadian(rot[0]), Engine::Util::ToRadian(rot[1]), Engine::Util::ToRadian(rot[2]));
		if(ImGui::InputFloat3("Scale", sca))
			transform.SetScale(sca[0] / 100.0f, sca[1] / 100.0f, sca[2] / 100.0f);

		ImGui::EndChild();

		ImGui::BeginChild("Material tab", ImVec2(350, 200), true);

		auto materialset = m_Model3[curModelIdx]->m_MaterialSet;
		
		const char* list[10];
		int i = 0;
		for (auto&[num, mat] : materialset->Materials)
		{
			list[i] = mat.Name.c_str();
			++i;
		}

		ImGui::Text("Material");
		ImGui::Combo("part", &selectedMat, list, materialset->Materials.size());
		
		auto& mat = materialset->Materials[selectedMat];
		ImGui::ColorEdit3("Ambient", mat.Ambient.m);
		ImGui::ColorEdit3("Diffuse", mat.Diffuse.m);
		ImGui::SliderFloat("Specular", &mat.Specular.x, 0.0f, 1.0f);
		ImGui::SliderFloat("Shiness", &mat.Shiness, 0.0f, 30.0f);

		ImGui::EndChild();
	}

	if (ImGui::CollapsingHeader("Light"))
	{
		int total = 0;
		const char* lightsList[20];

		static const char* types[]{ "Directional", "Point", "Spot" };
		for (auto& light : m_Lights)
		{
			lightsList[total] = light->name.c_str();
			++total;
		}

		auto curLight = m_Lights[selectedLight];
		ImGui::ListBox("Lights", &selectedLight, lightsList, total);
		if (ImGui::Button("Add", ImVec2(50, 20)))
			addLight = true;

		ImGui::BeginChild("LightTab", ImVec2(350, 200), true);
		int idx = int(curLight->m_Type);
		if (ImGui::Combo("Type", &idx, types, 3))
		{
			curLight->m_Type = static_cast<Engine::Light::Type>(idx);
		}

		ImGui::ColorEdit3("Color", curLight->m_Color.m);
		ImGui::SliderFloat("Intensity", &curLight->m_Intensity, 0.0f, 5.0f);

		switch (m_Lights[selectedLight]->m_Type)
		{
		case Engine::Light::Type::Directional:
			ImGui::SliderFloat3("Direction", curLight->lightCam.GetTransform().GetRotate().m, -3.14f, 3.14f);
			break;
		case Engine::Light::Type::Point:
			ImGui::SliderFloat("Range", &curLight->m_Range, 0.0f, 100.0f);
			ImGui::SliderFloat3("Position", curLight->lightCam.GetTransform().GetTranslate().m, -10.0f, 10.0f);
			break;
		case Engine::Light::Type::Spot:
			ImGui::SliderFloat("Range", &curLight->m_Range, 0.0f, 100.0f);
			ImGui::SliderFloat3("Position", curLight->lightCam.GetTransform().GetTranslate().m, -10.0f, 10.0f);
			ImGui::SliderFloat3("Direction", curLight->lightCam.GetTransform().GetRotate().m, -3.14f, 3.14f);
			ImGui::SliderFloat("InnerAngle", &curLight->m_InnerAngle, 0.0f, 1.54f);
			ImGui::SliderFloat("OuterAngle", &curLight->m_OuterAngle, 0.0f, 1.54f);
			break;
		}

		if (ImGui::Button("Delete", ImVec2(50, 20)))
			deleteLight = true;

		ImGui::EndChild();
	}

	ImGui::End();
}

