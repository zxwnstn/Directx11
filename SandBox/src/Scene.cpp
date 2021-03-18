#include "pceh.h"

#include "Scene.h"

#include "../../vendor/imgui/imgui.h"


void Scene::Add2DModel(std::shared_ptr<Engine::Model2D> model)
{
	m_Model2.push_back(model);
}

void Scene::Add3DModel(std::shared_ptr<Engine::Model3D> model)
{
	if (!model->m_Animation)
		curAnimtionIdx[model->m_Name] = 0;
	m_Model3.push_back(model);
}

void Scene::AddLight(std::shared_ptr<Engine::Light> light)
{
	m_Lights.push_back(light);
}

void Scene::AddCamera(std::shared_ptr<Engine::Camera> camera)
{
	m_Cameras.push_back(camera);
}

void Scene::SetSceneName(const std::string & name)
{
	m_Name = name;
}

Scene::Scene()
{
	//Cam
	float filedOfView = 3.141592f / 3.0f;
	std::shared_ptr<Engine::Camera> mainCam(new Engine::Camera(filedOfView, float(g_Width) / (float)g_Height, 0.1f, 1000.0f, "Main Camera"));
	mainCam->GetTransform().SetTranslate(0.0f, 2.0f, 4.0f);
	mainCam->GetTransform().SetRotate(0.0f, 3.14f, 0.0f);
	std::shared_ptr<Engine::Camera> subCam(new Engine::Camera(10, float(g_Width) / (float)g_Height, true, -1.0f, 1.0f, "Sub Camera"));
	std::shared_ptr<Engine::Light> light(new Engine::Light);
	
	light->m_Type = Engine::Light::Type::Directional;
	light->lightCam.GetTransform().SetRotate(0.8f, 0.0f, 0.0f);
	light->name = "Main Light";

	m_Curcam = mainCam;

	m_Cameras.push_back(mainCam);
	m_Cameras.push_back(subCam);
	m_Lights.push_back(light);
}



Scene::Scene(const std::string & name)
	: Scene()
{
	m_Name = name;
}

void Scene::OnUpdate(float dt)
{

	for(auto model : m_Model3)
		model->Update(dt);

	for(auto model : m_Model3)
		Engine::Renderer::Enque3D(model);

	for (auto model : m_Model2)
		Engine::Renderer::Enque2D(model);
}

void Scene::OnImGui()
{
	ImGui::Begin("Inspector");

	if (ImGui::CollapsingHeader("Camera"))
	{
		const char* camlist[10];
		int i = 0;
		for (auto& cam : m_Cameras)
			camlist[i++] = cam->GetName().c_str();

		ImGui::Combo("Camera list", &selectedCamera, camlist, m_Cameras.size());
		ImGui::BeginChild("CameraTab", ImVec2(350, 250), true);

		if (ImGui::Button("Set as main camera"))
		{
			m_Curcam = m_Cameras[selectedCamera];
			curcamIdx = selectedCamera;
		}

		auto curCam = m_Cameras[selectedCamera];
		const char* camtype[] = { "Ortho", "Perspective" };
		int _type = curCam->GetCamType();
		if (ImGui::Combo("Type", &_type, camtype, 2))
			curCam->SetType(static_cast<Engine::CameraType>(_type));

		float aspect = curCam->GetScreenAspect();
		ImGui::Text("Screen aspect : %f", aspect);

		if (_type == 0)
		{
			float mag = curCam->GetMag();
			if (ImGui::SliderFloat("Magnification", &mag, 0.1f, 100.0f))
				curCam->SetMagnification(mag);
		}
		else
		{
			float fov = curCam->GetFov();
			if (ImGui::SliderFloat("Field of view", &fov, 0.0f, 3.14f))
				curCam->SetFov(fov);
		}

		float _near = curCam->GetNear();
		if (ImGui::InputFloat("Near", &_near))
			curCam->SetNear(_near);

		float _far = curCam->GetFar();
		if (ImGui::InputFloat("Far", &_far))
			curCam->SetFar(_far);

		ImGui::Spacing();
		ImGui::Text("Camera Transform");

		auto& transform = curCam->GetTransform();

		auto& translate = transform.GetTranslate();
		auto& rotate = transform.GetRotate();
		float trans[3] = { translate.x, translate.y, translate.z };
		float rot[3] = { Engine::Util::ToDegree(rotate.x), Engine::Util::ToDegree(rotate.y), Engine::Util::ToDegree(rotate.z) };

		ImGui::InputFloat3("Translate", translate.m);
		if (ImGui::InputFloat3("Rotate(radian)", rot))
			transform.SetRotate(Engine::Util::ToRadian(rot[0]), Engine::Util::ToRadian(rot[1]), Engine::Util::ToRadian(rot[2]));

		ImGui::EndChild();
	}

	if (ImGui::CollapsingHeader("Model"))
	{
		const char* modelName[100];
		for (int i = 0; i < m_Model3.size(); ++i)
			modelName[i] = m_Model3[i]->m_Name.c_str();
		
		ImGui::Combo("model", &curModelIdx, modelName, m_Model3.size());

		if (ImGui::Button("Add New"))
			newModel = true;

		if (!m_Model3.empty())
		{
			auto curModel = m_Model3[curModelIdx];
			auto sekeltonName = curModel->GetSkeletonName();
			if (!sekeltonName.empty())
			{
				auto& animlist = curModel->GetAnimInfo()->AnimList;

				const char* animationList[100];

				animationList[0] = "T-Pose";
				for (int i = 0; i < animlist.size(); ++i)
					animationList[i + 1] = animlist[i].c_str();

				ImGui::BeginChild("AnimationTab", ImVec2(350, 150), true);
				ImGui::Text("Animation");
				std::string modelStr = curModel->m_Name;
				if (ImGui::Combo("anim", &curAnimtionIdx[modelStr], animationList, animlist.size() + 1))
				{
					if (curAnimtionIdx[modelStr] == 0)
					{
						m_Model3[curModelIdx]->StopAnimation();
					}
					else
					{
						m_Model3[curModelIdx]->PlayAnimation();
						m_Model3[curModelIdx]->SetAnimation(animlist[curAnimtionIdx[curModel->m_Name] - 1], true);
					}
				}

				if (curAnimtionIdx[modelStr] != 0)
				{
					auto& animationInfom = curModel->GetAnimInfo();
					if (ImGui::Checkbox("Loop", &animationInfom->Loop))
					{
						if (animationInfom->Loop)
							animationInfom->Expired = false;
					}
					ImGui::SliderFloat("x Play Speed", &animationInfom->Accelation, 0.0f, 5.0f);
					float duration = animationInfom->Duration;
					ImGui::SliderFloat("Current time", &animationInfom->Elapsedtime, 0.0f, duration);
				}
				ImGui::EndChild();
			}

			ImGui::BeginChild("TRSTab", ImVec2(350, 100), true);
			auto& transform = m_Model3[curModelIdx]->m_Transform;

			auto& translate = transform.GetTranslate();
			auto& rotate = transform.GetRotate();
			auto& scale = transform.GetScale();
			float trans[3] = { translate.x, translate.y, translate.z };
			float rot[3] = { Engine::Util::ToDegree(rotate.x), Engine::Util::ToDegree(rotate.y), Engine::Util::ToDegree(rotate.z) };


			ImGui::Text("Model Transform");
			ImGui::InputFloat3("Translate", translate.m);
			if (ImGui::InputFloat3("Rotate(degree)", rot))
				transform.SetRotate(Engine::Util::ToRadian(rot[0]), Engine::Util::ToRadian(rot[1]), Engine::Util::ToRadian(rot[2]));
			if (curModel->m_Animation)
			{
				float sca[3] = { scale.x * 100, scale.y * 100, scale.z * 100 };
				if (ImGui::InputFloat3("Scale", sca))
					transform.SetScale(sca[0] / 100.0f, sca[1] / 100.0f, sca[2] / 100.0f);
			}
			else
			{
				float sca[3] = { scale.x, scale.y, scale.z };
				if (ImGui::InputFloat3("Scale", sca))
					transform.SetScale(sca[0], sca[1], sca[2]);
			}

			ImGui::EndChild();

			ImGui::BeginChild("Material tab", ImVec2(350, 250), true);

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
			ImGui::ColorEdit3("Specular", mat.Specular.m);
			ImGui::SliderFloat("Shiness", &mat.Shiness, 0.1f, 30.0f);

			ImGui::Text("Material Mapping Mode");
			int mappingMode = mat.MapMode;
			bool diffuseMap = 1 & mappingMode;
			bool normalMap = 2 & mappingMode;
			bool specularMap = 4 & mappingMode;
			ImGui::Checkbox("DiffuseTexture", &diffuseMap);
			ImGui::Checkbox("NormalTexture", &normalMap);
			ImGui::Checkbox("SpecularTexture", &specularMap);

			mat.MapMode = (int)diffuseMap | (int)normalMap * 2 | (int)specularMap * 4;

			ImGui::EndChild();

			if (ImGui::Button("delete"))
				deleteModel = true;
		}
	}

	static const char* ltypes[]{ "Directional", "Point", "Spot" };
	if (ImGui::CollapsingHeader("Light"))
	{
		int total = 0;
		const char* lightsList[20];
		
		for (auto& light : m_Lights)
		{
			lightsList[total] = light->name.c_str();
			++total;
		}

		ImGui::Combo("Lights", &selectedLight, lightsList, total);
		if (ImGui::Button("Add", ImVec2(50, 20)))
		{
			newLight = true;		
		}

		if (total)
		{
			auto curLight = m_Lights[selectedLight];
			ImGui::BeginChild("LightTab", ImVec2(350, 200), true);
			int idx = int(curLight->m_Type);
			if (ImGui::Combo("Type", &idx, ltypes, 3))
			{
				curLight->m_Type = static_cast<Engine::Light::Type>(idx);
			}

			ImGui::ColorEdit3("Color", curLight->m_Color.m);
			ImGui::SliderFloat("Intensity", &curLight->m_Intensity, 0.0f, 5.0f);

			switch (m_Lights[selectedLight]->m_Type)
			{
			case Engine::Light::Type::Directional:
			{
				auto& cascademat = curLight->m_CascadedMat;
				ImGui::SliderFloat3("Direction", curLight->lightCam.GetTransform().GetRotate().m, -3.14f, 3.14f);
				ImGui::SliderFloat("Cascade border1", &cascademat.m_arrCascadeRanges[1], 0.2f, 100.0f);
				ImGui::SliderFloat("Cascade border2", &cascademat.m_arrCascadeRanges[2], 0.3f, 100.1f);
				if (cascademat.m_arrCascadeRanges[1] >= cascademat.m_arrCascadeRanges[2])
					cascademat.m_arrCascadeRanges[2] = cascademat.m_arrCascadeRanges[1] + 0.1f;
			}
			break;
			case Engine::Light::Type::Point:
				ImGui::SliderFloat("Range", &curLight->m_Range, 0.0f, 100.0f);
				ImGui::SliderFloat3("Position", curLight->lightCam.GetTransform().GetTranslate().m, -10.0f, 10.0f);
				break;
			case Engine::Light::Type::Spot:
				ImGui::SliderFloat("Range", &curLight->m_Range, 0.0f, 100.0f);
				ImGui::SliderFloat3("Position", curLight->lightCam.GetTransform().GetTranslate().m, -10.0f, 10.0f);
				ImGui::SliderFloat3("Direction", curLight->lightCam.GetTransform().GetRotate().m, -3.14f, 3.14f);
				ImGui::SliderFloat("InnerAngle", &curLight->m_InnerAngle, 0.1f, 1.54f);
				ImGui::SliderFloat("OuterAngle", &curLight->m_OuterAngle, 0.1f, 1.55f);
				if (curLight->m_InnerAngle > curLight->m_OuterAngle)
					curLight->m_OuterAngle = curLight->m_InnerAngle + 0.1f;

				break;
			}

			if (ImGui::Button("Delete", ImVec2(50, 20)))
				deleteLight = true;

			ImGui::EndChild();
		}
	}

	ImGui::End();

	if (newModel)
	{
		ImGui::Begin("Create New Model");

		auto staticList = Engine::MeshArchive::GetStaticMeshList();
		auto skeletalList = Engine::MeshArchive::GetSkeletalMeshList();

		const char* type[] = { "Skeletal", "Static" };
		if (ImGui::Combo("Type", &newModelType, type, 2)) 
		{
			if (newModelType)
			{
				selectedStatic = 0;
				selectedName = staticList[selectedStatic];
			}
			else
			{
				selectedSkeletal = 0;
				selectedName = skeletalList[selectedSkeletal];
			}
		}
		
		const char* StaticModels[100];
		const char* SkeltalModels[100];
		if (newModelType)
		{
			int i = 0;
			for (auto& name : staticList)
				StaticModels[i++] = name.c_str();

			if (ImGui::Combo("Mesh", &selectedStatic, StaticModels, staticList.size()))
				selectedName = staticList[selectedStatic];

			if (selectedName.empty()) selectedName = staticList[0];
		}
		else 
		{
			int i = 0;
			for (auto& name : skeletalList)
				SkeltalModels[i++] = name.c_str();

			if(ImGui::Combo("Mesh", &selectedStatic, SkeltalModels, skeletalList.size()))
				selectedName = skeletalList[selectedStatic];

			if (selectedName.empty()) selectedName = skeletalList[0];
		}
		ImGui::InputText("Name", newModelBuffer, 100);

		if (ImGui::Button("Create"))
		{
			if (newModelType)
			{
				std::shared_ptr<Engine::Model3D> model = Engine::Model3D::Create()
					.buildFromOBJ()
					.SetObject(selectedName);
				model->m_Name = newModelBuffer;
				m_Model3.push_back(model);
			}
			else
			{
				std::shared_ptr<Engine::Model3D> model = Engine::Model3D::Create()
					.buildFromFBX()
					.SetSkeleton(selectedName);
				model->m_Transform.SetScale(0.01f, 0.01f, 0.01f);
				model->m_Name = newModelBuffer;
				m_Model3.push_back(model);
			}
			memset(newModelBuffer, 0, 100);
			newModel = false;
			curModelIdx = m_Model3.size() - 1;
			selectedName.clear();
		}
		if (ImGui::Button("Cancel"))
		{
			memset(newModelBuffer, 0, 100);
			newModel = false;
			selectedName.clear();
		}
		ImGui::End();
	}

	if (newLight)
	{
		ImGui::Begin("Create Light");

		ImGui::Combo("Type", &newlightType, ltypes, 3);
		ImGui::InputText("Name", newLightBuffer, 100);
		ImGui::InputFloat3("Position", newLightPosition);

		if (ImGui::Button("Create"))
		{
			std::shared_ptr<Engine::Light> light(new Engine::Light);
			switch (newlightType)
			{
			case 0: light->m_Type = Engine::Light::Type::Directional;
			case 1: light->m_Type = Engine::Light::Type::Point;
			case 2: light->m_Type = Engine::Light::Type::Spot;
			}
			light->lightCam.GetTransform().SetTranslate(newLightPosition[0], newLightPosition[1], newLightPosition[2]);
			light->name = newLightBuffer;
			m_Lights.push_back(light);
			memset(newLightBuffer, 0, 100);
			memset(newLightPosition, 0, 12);
			newlightType = 0;
			newLight = false;
		}
		if (ImGui::Button("Cancel"))
		{
			memset(newLightBuffer, 0, 100);
			memset(newLightPosition, 0, 12);
			newlightType = 0;
			newLight = false;
		}
		ImGui::End();
	}

	if (deleteModel)
	{
		m_Model3.erase(m_Model3.begin() + curModelIdx);
		if (curModelIdx == m_Model3.size())
			--curModelIdx;
		deleteModel = false;
	}

	if (deleteLight)
	{
		deleteLight = false;
		m_Lights.erase(m_Lights.begin() + selectedLight);
		if (selectedLight == m_Lights.size())
			--selectedLight;
	}

}

SceneInform Scene::Save()
{
	SceneInform inform;
	inform.SceneName = m_Name;
	inform.CurrentCamIdx = curcamIdx;
	for (auto cam : m_Cameras)
	{
		CameraInform camInform;
		camInform.Name = cam->GetName();
		camInform.Far = cam->GetFar();
		camInform.Near = cam->GetNear();
		camInform.Mag = cam->GetMag();
		camInform.Fov = cam->GetFov();
		camInform.Rotate = cam->GetTransform().GetRotate();
		camInform.Translate = cam->GetTransform().GetTranslate();
		camInform.Type = cam->GetCamType();
		inform.Camera.push_back(camInform);
	}
	for (auto model : m_Model3)
	{
		Model3DInform modelInform;
		modelInform.Name = model->m_Name;
		modelInform.MeshName = model->m_MeshName;
		{
			auto anim = model->GetAnimInfo();
			modelInform.MeshType = 1;
			if (anim)
			{
				modelInform.MeshType = 0;

				int animIdx = curAnimtionIdx[model->m_Name];

				if (animIdx)
				{
					modelInform.AnimInfo.CurAnimation = anim->AnimList[animIdx - 1];
				}
				else
				{
					modelInform.AnimInfo.CurAnimation = "T-Pose";
				}
				modelInform.AnimInfo.Curtime = anim->Elapsedtime;
				modelInform.AnimInfo.Loop = anim->Loop;
				modelInform.AnimInfo.PlaySpeed = anim->Accelation;
			}
		}
		modelInform.Translate = model->m_Transform.GetTranslate();
		modelInform.Scale = model->m_Transform.GetScale();
		modelInform.Rotate = model->m_Transform.GetRotate();

		auto material = model->m_MaterialSet;
		for (int i = 0; i < material->Materials.size(); ++i)
		{
			MaterialInform matInform;
			matInform.Name = material->Materials[i].Name;
			matInform.Ambient = material->Materials[i].Ambient;
			matInform.Diffuse = material->Materials[i].Diffuse;
			matInform.Mapmode = material->Materials[i].MapMode;
			matInform.Specular = material->Materials[i].Specular;
			matInform.Shiness = material->Materials[i].Shiness;
			modelInform.Material.push_back(matInform);
		}

		inform.Model3d.push_back(modelInform);
	}
	for (auto light : m_Lights)
	{
		LightInform lightInform;

		lightInform.Name = light->name;
		lightInform.type = static_cast<int>(light->m_Type);
		lightInform.CasBorder1 = light->m_CascadedMat.m_arrCascadeRanges[1];
		lightInform.CasBorder2 = light->m_CascadedMat.m_arrCascadeRanges[2];
		lightInform.Color = light->m_Color;
		lightInform.Direction = light->lightCam.GetTransform().GetRotate();
		lightInform.InnerAngle = light->m_InnerAngle;
		lightInform.OuterAngle = light->m_OuterAngle;
		lightInform.Position = light->lightCam.GetTransform().GetTranslate();
		lightInform.Range = light->m_Range;
		lightInform.Intensity = light->m_Intensity;

		inform.Light.push_back(lightInform);
	}
	return inform;
}

Scene::Scene(const SceneInform & _inform)
{
	m_Name = _inform.SceneName;
	for (auto& inform : _inform.Camera)
	{
		float screenAspect = (float)g_Width / (float)g_Height;
		std::shared_ptr<Engine::Camera> cam(new Engine::Camera(inform.Fov, screenAspect));
		cam->SetFar(inform.Far);
		cam->SetFov(inform.Fov);
		cam->SetMagnification(inform.Mag);
		cam->SetName(inform.Name);
		cam->SetNear(inform.Near);
		cam->GetTransform().SetRotate(inform.Rotate);
		cam->GetTransform().SetTranslate(inform.Translate);
		cam->SetType(static_cast<Engine::CameraType>(inform.Type));
		cam->UpdateViewProj();
		m_Cameras.push_back(cam);
	}
	m_Curcam = m_Cameras[_inform.CurrentCamIdx];

	for (auto& inform : _inform.Model3d)
	{
		std::shared_ptr<Engine::Model3D> model;
		if (inform.MeshType == 0)
		{
			if (!Engine::MeshArchive::HasSkeletalMesh(inform.MeshName))
			{
				LOG_CRITICAL("LoadScene::There is no cashed skeletal mesh! {0}", inform.MeshName);
				continue;
			}

			model = Engine::Model3D::Create()
				.buildFromFBX()
				.SetSkeleton(inform.MeshName);

			auto animInform = model->GetAnimInfo();
			curAnimtionIdx[model->m_Name] = 0;
			if (animInform)
			{
				bool findCurAnim = false;
				int animIdx = 1;
				for (auto anim : animInform->AnimList)
				{
					if (anim == inform.AnimInfo.CurAnimation)
					{
						animInform->CurAnim = inform.AnimInfo.CurAnimation;
						findCurAnim = true;
						curAnimtionIdx[model->m_Name] = animIdx;
						break;
					}
					++animIdx;
				}
				if (!findCurAnim)
				{
					model->StopAnimation();
					LOG_CRITICAL("LoadScene::There is no cashed animation {0}'s {1}", inform.MeshName, inform.AnimInfo.CurAnimation);
				}
				else
				{
					model->PlayAnimation();
					model->SetAnimation(inform.AnimInfo.CurAnimation, true);
				}

				
				animInform->Elapsedtime = inform.AnimInfo.Curtime;
				animInform->Loop = inform.AnimInfo.Loop;
				animInform->Accelation = inform.AnimInfo.PlaySpeed;
			}
		}
		else
		{
			if (!Engine::MeshArchive::HasStaticMesh(inform.MeshName))
			{
				LOG_CRITICAL("LoadScene::There is no cashed static mesh! {0}", inform.MeshName);
				continue;
			}

			model = Engine::Model3D::Create()
				.buildFromOBJ()
				.SetObject(inform.MeshName);
		}
		
		model->m_Name = inform.Name;
		model->m_MaterialSet = Engine::MaterialArchive::GetSetCopy(model->m_MeshName);
		auto materialset = model->m_MaterialSet;
		for (int i = 0; i < inform.Material.size(); ++i)
		{
			auto& mat = materialset->Materials[i];
			mat.Ambient = inform.Material[i].Ambient;
			mat.Diffuse = inform.Material[i].Diffuse;
			mat.Specular = inform.Material[i].Specular;
			mat.MapMode = inform.Material[i].Mapmode;
			mat.Shiness = inform.Material[i].Shiness;
		}
		
		model->m_Transform.SetTranslate(inform.Translate);
		model->m_Transform.SetRotate(inform.Rotate);
		model->m_Transform.SetScale(inform.Scale);
		
		m_Model3.push_back(model);
	}

	for (auto& inform : _inform.Light)
	{
		std::shared_ptr<Engine::Light> light(new Engine::Light);
		light->name = inform.Name;
		light->m_CascadedMat.m_arrCascadeBoundRadius[1] = inform.CasBorder1;
		light->m_CascadedMat.m_arrCascadeBoundRadius[2] = inform.CasBorder2;
		light->m_Color = inform.Color;
		light->lightCam.GetTransform().SetRotate(inform.Direction);
		light->m_InnerAngle = inform.InnerAngle;
		light->m_Intensity = inform.Intensity;
		light->m_OuterAngle = inform.OuterAngle;
		light->lightCam.GetTransform().SetTranslate(inform.Position);
		light->m_Range = inform.Range;
		light->m_Type = static_cast<Engine::Light::Type>(inform.type);
		m_Lights.push_back(light);
	}

}