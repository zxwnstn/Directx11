#include "pceh.h"

#include "Scene.h"

#include "../../vendor/imgui/imgui.h"

#include "Script/MultiLighting.h"

void Scene::Add2DModel(std::shared_ptr<Engine::Model2D> model)
{
	m_Model2.push_back(model);
	LOG_TRACE("On {0} scene added 2D-model", m_Name);
}

void Scene::Add3DModel(std::shared_ptr<Engine::Model3D> model)
{
	if (!model->m_Animation)
		curAnimtionIdx[model->m_Name] = 0;
	m_Model3.push_back(model);
	LOG_TRACE("On {0} scene added 3D-model : {1}", m_Name, model->m_Name);
}

void Scene::AddLight(std::shared_ptr<Engine::Light> light)
{
	LOG_TRACE("On {0} scene added light : {1}", m_Name, light->name);
	m_Lights.push_back(light);
}

void Scene::AddCamera(std::shared_ptr<Engine::Camera> camera)
{
	LOG_TRACE("On {0} scene added Camera : {1}", m_Name, camera->GetName());
	m_Cameras.push_back(camera);
}

std::shared_ptr<Engine::Model3D> Scene::GetModel3d(const std::string & name)
{
	for (auto model : m_Model3)
	{
		if (model->m_Name == name)
		{
			return model;
		}
	}
	LOG_WARN("Can't find {0} model on {1} scene ", name, m_Name);
	return nullptr;
}

std::shared_ptr<Engine::Light> Scene::GetLight(const std::string & name)
{
	for (auto model : m_Lights)
	{
		if (model->name == name)
		{
			return model;
		}
	}
	LOG_WARN("Can't find {0} model on {1} scene ", name, m_Name);
	return nullptr;
}

void Scene::SetSceneName(const std::string & name)
{
	LOG_TRACE("On {0} scene renamed into {1}", m_Name, name);
	m_Name = name;
}

Scene::Scene(const std::string & name)
	: Scene()
{
	m_Name = name;
}

Scene::Scene()
{
	//floor
	std::shared_ptr<Engine::Model3D> floor = Engine::Model3D::Create()
		.buildFromOBJ()
		.SetObject("Square");

	floor->m_Name = "floor";
	floor->m_Transform.SetScale(10.0f, 1.0f, 10.0f);

	//skybox
	std::shared_ptr<Engine::Model3D> skybox = Engine::Model3D::Create()
		.buildFromOBJ()
		.SetObject("SkyBox");

	skybox->m_Name = "SkyBox";
	skybox->m_Transform.SetScale(100.0f, 100.0f, 100.0f);
	skybox->m_MaterialSet->MaterialTextures[0][0].Name = "skyBox";


	//Cam
	float filedOfView = 3.141592f / 3.0f;
	std::shared_ptr<Engine::Camera> mainCam(new Engine::Camera(filedOfView, float(g_Width) / (float)g_Height, 0.1f, 1000.0f, "Main Camera"));
	mainCam->GetTransform().SetTranslate(0.0f, 13.0f, -14.0f);
	mainCam->GetTransform().SetRotate(35.0f / 180.0f * 3.141592f, 0.0f, 0.0f);
	std::shared_ptr<Engine::Camera> subCam(new Engine::Camera(10, float(g_Width) / (float)g_Height, true, -1.0f, 1.0f, "Sub Camera"));
	std::shared_ptr<Engine::Light> light(new Engine::Light);
	
	//light
	light->m_Type = Engine::Light::Type::Directional;
	light->lightCam.GetTransform().SetRotate(Engine::Util::ToRadian(120.0f), Engine::Util::ToRadian(35.0f), 0.0f);
	light->name = "Main Light";

	m_Curcam = mainCam;

	m_Cameras.push_back(mainCam);
	m_Cameras.push_back(subCam);
	m_Lights.push_back(light);
	m_Model3.push_back(floor);
	m_Model3.push_back(skybox);
}

void Scene::OnMouseMove(float dx, float dy, float sensitive)
{
	if (runScript)
	{
		m_MyScript->OnMouseMove(dx * sensitive, dy * sensitive);
	}
	else
	{
		auto& camTransform = GetCurCam()->GetTransform();
		if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
		{
			dx *= sensitive;
			dy *= sensitive;
			camTransform.LocalRotateX(dx);
			camTransform.LocalRotateY(dy);
		}
	}

}

void Scene::OnKeyInput()
{
	if (runScript)
	{
		m_MyScript->OnKeyInput();
	}
	else
	{
		auto& camTransform = GetCurCam()->GetTransform();
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

		if (GetAsyncKeyState('E') & 0x8000)
		{
			camTransform.AddTranslate(0.0f, 0.1f, 0.0f);
		}
		if (GetAsyncKeyState('Q') & 0x8000)
		{
			camTransform.AddTranslate(0.0f, -0.1f, 0.0f);
		}
	}
}

void Scene::OnUpdate(float dt)
{
	auto globalenv = Engine::Renderer::GetGlobalEnv();
	Engine::Renderer::GetSkyColor() = m_worldInform.SkyColor;
	globalenv->WorldMatrix = Engine::Util::GetTransform(m_worldInform.WorldTranslate, m_worldInform.WorldRotate, m_worldInform.WorldScale, true);
	globalenv->Ambient.x = m_worldInform.GlobalAmbient.x;
	globalenv->Ambient.y = m_worldInform.GlobalAmbient.y;
	globalenv->Ambient.z = m_worldInform.GlobalAmbient.z;

	OnKeyInput();
	if (runScript && !pauseScript)
	{
		m_MyScript->OnUpdate(dt);
	}

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

	if (ImGui::CollapsingHeader("World"))
	{
		ImGui::BeginChild("World Tab", ImVec2(350, 150), true);
		ImGui::BulletText("World Transform");
		ImGui::InputFloat3("Translate", m_worldInform.WorldTranslate.m);
		Engine::vec3 temp;
		{
			temp.x = Engine::Util::ToDegree(m_worldInform.WorldRotate.x);
			temp.y = Engine::Util::ToDegree(m_worldInform.WorldRotate.y);
			temp.z = Engine::Util::ToDegree(m_worldInform.WorldRotate.z);
		}
		if (ImGui::SliderFloat3("Rotate(degree)", temp.m, 0.0f, 360.0f))
		{
			m_worldInform.WorldRotate.x = Engine::Util::ToRadian(temp.x);
			m_worldInform.WorldRotate.y = Engine::Util::ToRadian(temp.y);
			m_worldInform.WorldRotate.z = Engine::Util::ToRadian(temp.z);
		}
		ImGui::SliderFloat3("Scale", m_worldInform.WorldScale.m, 0.1f, 1.0f);

		ImGui::ColorEdit3("SkyColor", m_worldInform.SkyColor.m);
		ImGui::ColorEdit3("Global Ambient", m_worldInform.GlobalAmbient.m);

		ImGui::EndChild();
	}

	if (ImGui::CollapsingHeader("Camera"))
	{
		const char* camlist[10];
		int i = 0;
		for (auto& cam : m_Cameras)
			camlist[i++] = cam->GetName().c_str();

		ImGui::Combo("Camera list", &selectedCamera, camlist, (int)m_Cameras.size());
		ImGui::BeginChild("CameraTab", ImVec2(350, 250), true);

		if (ImGui::Button("New Camera"))
			newCam = true;

		ImGui::SameLine();
		if(m_Cameras.size() > 1)
		{
			if (ImGui::Button("delete"))
			{
				if (activateCamIdx >= selectedCamera)
					--activateCamIdx;

				std::string camname = m_Cameras[selectedCamera]->GetName();
				m_Cameras.erase(m_Cameras.begin() + selectedCamera);
				if (m_Cameras.size() == selectedCamera)
					--selectedCamera;

				m_Curcam = m_Cameras[activateCamIdx];

				LOG_TRACE("On {0} scene deleted 3D-model : {1}", m_Name, camname);
			}
		}
		ImGui::Spacing();
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
			if (ImGui::SliderFloat("Field of view", &fov, 0.1f, 3.14f))
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
		float rot[3] = { Engine::Util::ToDegree(rotate.x), Engine::Util::ToDegree(rotate.y), Engine::Util::ToDegree(rotate.z) };
		
		ImGui::InputFloat3("Translate", translate.m);
		if (ImGui::SliderFloat3("Rotate(Degree)", rot, 0.0f, 360.0f))
			transform.SetRotate(Engine::Util::ToRadian(rot[0]), Engine::Util::ToRadian(rot[1]), Engine::Util::ToRadian(rot[2]));

		if (ImGui::Button("Set as main camera"))
		{
			m_Curcam = m_Cameras[selectedCamera];
			activateCamIdx = selectedCamera;
		}

		ImGui::EndChild();
	}

	if (ImGui::CollapsingHeader("Model"))
	{
		const char* modelName[100];
		for (int i = 0; i < m_Model3.size(); ++i)
			modelName[i] = m_Model3[i]->m_Name.c_str();
		
		ImGui::Combo("model", &curModelIdx, modelName, (int)m_Model3.size());

		if (ImGui::Button("New Model"))
			newModel = true;
		
		if (!m_Model3.empty())
		{
			ImGui::SameLine();
			if (ImGui::Button("delete"))
			{
				std::string modelName = m_Model3[curModelIdx]->m_Name;
				m_Model3.erase(m_Model3.begin() + curModelIdx);
				if (curModelIdx == m_Model3.size())
					--curModelIdx;
				deleteModel = false;
				LOG_TRACE("On {0} scene deleted 3D-model : {1}", m_Name, modelName);
			}
		}

		if (!m_Model3.empty())
		{
			auto curModel = m_Model3[curModelIdx];
			auto meshType = curModel->GetMeshType();
			if (meshType == 0)
			{
				auto& animlist = curModel->GetAnimInfo()->AnimList;

				const char* animationList[100];

				animationList[0] = "T-Pose";
				for (int i = 0; i < animlist.size(); ++i)
					animationList[i + 1] = animlist[i].c_str();

				ImGui::BeginChild("AnimationTab", ImVec2(350, 130), true);
				ImGui::Text("Animation");
				std::string modelStr = curModel->m_Name;
				if (ImGui::Combo("anim", &curAnimtionIdx[modelStr], animationList, (int)animlist.size() + 1))
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
			if (ImGui::SliderFloat3("Rotate(degree)", rot, 0.0f, 360.0f))
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
			ImGui::Combo("part", &selectedMat, list, (int)materialset->Materials.size());

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
			if(!materialset->MaterialTextures[selectedMat][0].Name.empty())
				ImGui::Checkbox("DiffuseTexture", &diffuseMap);
			if (!materialset->MaterialTextures[selectedMat][1].Name.empty())
				ImGui::Checkbox("NormalTexture", &normalMap);
			if (!materialset->MaterialTextures[selectedMat][2].Name.empty())
				ImGui::Checkbox("SpecularTexture", &specularMap);

			mat.MapMode = (int)diffuseMap | (int)normalMap * 2 | (int)specularMap * 4;

			static bool ChangeTexture = false;
			if (ImGui::Button("Mapping Texture change"))
			{
				ChangeTexture = true;
			}

			if (ChangeTexture)
			{
				ImGui::Begin("Select texture");
				auto list = Engine::TextureArchive::GetTextureList();
				const char* target[] = { "DiffuseMap", "NormalMap", "SpecularMap" };
				const char* textures[100];
				static int curTarget = 0;
				static int curTexture = 0;

				int i = 0;
				for (auto it = list.begin(); it != list.end(); ++it)
				{
					if (*it == "BackBuffer" || *it == "HDRTexture" || *it == "ForwardTexture" || *it == "images")
						continue;
					textures[i++] = it->c_str();
				}

				auto currentTextureNamestr = materialset->MaterialTextures[selectedMat][curTarget].Name;
				const char* currentTextureName;
				if (currentTextureNamestr.empty())
					currentTextureName = "none";
				else
					currentTextureName = currentTextureNamestr.c_str();

				ImGui::BulletText("Current texture %s", currentTextureName);
				ImGui::Combo("Target", &curTarget, target, 3);
				ImGui::Combo("Texture", &curTexture, textures, i);

				if (ImGui::Button("Ok"))
				{
					materialset->MaterialTextures[selectedMat][curTarget].Name = textures[curTexture];
					curTarget = 0;
					curTexture = 0;
					ChangeTexture = false;
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel"))
				{
					curTarget = 0;
					curTexture = 0;
					ChangeTexture = false;
				}
				ImGui::End();
			}

			ImGui::EndChild();
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
		if (ImGui::Button("New Light"))
		{
			newLight = true;		
		}

		if (total)
		{
			ImGui::SameLine();
			if (ImGui::Button("Delete", ImVec2(50, 20)))
				deleteLight = true;

			auto curLight = m_Lights[selectedLight];
			ImGui::BeginChild("LightTab", ImVec2(350, 400), true);
			int idx = int(curLight->m_Type);
			if (ImGui::Combo("Type", &idx, ltypes, 3))
			{
				curLight->SetType(static_cast<Engine::Light::Type>(idx));
			}

			ImGui::ColorEdit3("Color", curLight->m_Color.m);

			float renderscale = curLight->lightCam.GetTransform().GetScale().x * 5.0f;
			if(ImGui::SliderFloat("RenderScale", &renderscale, 0.0f, 10.0f))
				curLight->lightCam.GetTransform().SetScale(renderscale * 0.2f, renderscale * 0.2f, renderscale * 0.2f);

			ImGui::SliderFloat("Intensity", &curLight->m_Intensity, 0.0f, 5.0f);
			
			switch (m_Lights[selectedLight]->m_Type)
			{
			case Engine::Light::Type::Directional:
			{

				auto& rotate = curLight->lightCam.GetTransform().GetRotate();
				float rot[] = { Engine::Util::ToDegree(rotate.x),  Engine::Util::ToDegree(rotate.y) ,  Engine::Util::ToDegree(rotate.z) };
				if (ImGui::SliderFloat3("Direction(Degree)", rot, 0.0f, 360.0f))
				{
					rotate.x = Engine::Util::ToRadian(rot[0]);
					rotate.y = Engine::Util::ToRadian(rot[1]);
					rotate.z = Engine::Util::ToRadian(rot[2]);
				}			
			}
			break;
			case Engine::Light::Type::Point:
				ImGui::SliderFloat("Range", &curLight->m_Range, 0.0f, 100.0f);
				ImGui::InputFloat3("Position", curLight->lightCam.GetTransform().GetTranslate().m);
				break;
			case Engine::Light::Type::Spot:
			{
				ImGui::SliderFloat("Range", &curLight->m_Range, 0.0f, 100.0f);
				ImGui::InputFloat3("Position", curLight->lightCam.GetTransform().GetTranslate().m);
				
				auto rotate = curLight->lightCam.GetTransform().GetRotate();
				float rot[] = { Engine::Util::ToDegree(rotate.x),  Engine::Util::ToDegree(rotate.y) ,  Engine::Util::ToDegree(rotate.z) };
				if(ImGui::SliderFloat3("Direction", curLight->lightCam.GetTransform().GetRotate().m, 0.0f, 360.0f))
				{
					rotate.x = Engine::Util::ToRadian(rot[0]);
					rotate.y = Engine::Util::ToRadian(rot[1]);
					rotate.z = Engine::Util::ToRadian(rot[2]);
				}
				ImGui::SliderFloat("InnerAngle", &curLight->m_InnerAngle, 0.1f, 1.54f);
				
				static float outerAng = 1.0f;
				outerAng = curLight->m_OuterAngle;
				if (ImGui::SliderFloat("OuterAngle", &outerAng, 0.1f, 1.55f))
					curLight->SetOuterAngle(outerAng);

				if (curLight->m_InnerAngle > curLight->m_OuterAngle)
					curLight->m_OuterAngle = curLight->m_InnerAngle + 0.1f;
			}
				break;
			}

			if(ImGui::TreeNode("Light Cam Detail"))
			{
				static float n, a, m;
				n = curLight->lightCam.GetNear();
				if (ImGui::InputFloat("Near", &n, 0.1f, 1.0f))
					curLight->lightCam.SetNear(n);

				a = curLight->lightCam.GetScreenAspect();
				if (ImGui::InputFloat("ScreenAspect", &a, 0.1f, 1.0f))
					curLight->lightCam.OnResize(a, 1.0f);

				m = curLight->lightCam.GetMag();
				if (ImGui::InputFloat("Magnification", &m, 0.1f, 1.0f))
					curLight->lightCam.SetMagnification(m);

				static float f;
				f = curLight->lightCam.GetFov();
				if (ImGui::InputFloat("Fov", &f, 0.1f, 1.0f))
					curLight->lightCam.SetFov(f);

				auto& cascademat = curLight->m_CascadedMat;
				ImGui::SliderFloat("Cascade Shadow border1", &cascademat.m_arrCascadeRanges[1], 0.2f, 100.0f);
				ImGui::SliderFloat("Cascade Shadow border2", &cascademat.m_arrCascadeRanges[2], 0.3f, 100.1f);
				if (cascademat.m_arrCascadeRanges[1] >= cascademat.m_arrCascadeRanges[2])
					cascademat.m_arrCascadeRanges[2] = cascademat.m_arrCascadeRanges[1] + 0.1f;

				ImGui::TreePop();
			}
			
			ImGui::EndChild();
		}
	}

	if (ImGui::CollapsingHeader("Script"))
	{
		if (m_MyScript)
		{
			ImGui::Text("Current script : %s", m_MyScript->m_Name.c_str());
			if (!runScript)
			{
				if (ImGui::Button("Run Script"))
				{
					m_MyScript->OnStart();
					runScript = true;
				}
			}
			else
			{
				if (!pauseScript)
				{
					if (ImGui::Button("Pasue"))
					{
						pauseScript = true;
					}
				}
				else
				{
					if (ImGui::Button("Resume"))
					{
						pauseScript = false;
					}
				}
				
				if (ImGui::Button("Stop"))
				{
					m_MyScript->OnStop();
					runScript = false;
					pauseScript = false;
				}
			}
		}
		else
		{
			ImGui::Text("Current script : None");
		}
		if (!runScript)
		{
			if (ImGui::Button("SetScript"))
				setScript = true;
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

			if (ImGui::Combo("Mesh", &selectedStatic, StaticModels, (int)staticList.size()))
				selectedName = staticList[selectedStatic];

			if (selectedName.empty()) selectedName = staticList[0];
		}
		else 
		{
			int i = 0;
			for (auto& name : skeletalList)
				SkeltalModels[i++] = name.c_str();

			if(ImGui::Combo("Mesh", &selectedStatic, SkeltalModels, (int)skeletalList.size()))
				selectedName = skeletalList[selectedStatic];

			if (selectedName.empty()) selectedName = skeletalList[0];
		}
		ImGui::InputText("Name", newModelBuffer, 100);

		if (ImGui::Button("Create"))
		{
			std::shared_ptr<Engine::Model3D> model;
			if (newModelType)
			{
				model = Engine::Model3D::Create()
					.buildFromOBJ()
					.SetObject(selectedName);
				model->m_Name = newModelBuffer;
			}
			else
			{
				model = Engine::Model3D::Create()
					.buildFromFBX()
					.SetSkeleton(selectedName);
				model->m_Transform.SetScale(0.01f, 0.01f, 0.01f);
				model->m_Name = newModelBuffer;
			}
			Add3DModel(model);

			memset(newModelBuffer, 0, 100);
			newModel = false;
			curModelIdx = (int)m_Model3.size() - 1;
			selectedName.clear();
			selectedStatic = 0;
			selectedSkeletal = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			memset(newModelBuffer, 0, 100);
			newModel = false;
			selectedName.clear();
			selectedStatic = 0;
			selectedSkeletal = 0;
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
			case 0: light->m_Type = Engine::Light::Type::Directional; break;
			case 1: light->m_Type = Engine::Light::Type::Point; break;
			case 2: light->m_Type = Engine::Light::Type::Spot; break;
			}
			light->lightCam.GetTransform().SetTranslate(newLightPosition[0], newLightPosition[1], newLightPosition[2]);
			light->name = newLightBuffer;
			AddLight(light);
			selectedLight = (int)m_Lights.size() - 1;
			memset(newLightBuffer, 0, 100);
			memset(newLightPosition, 0, 12);
			newlightType = 0;
			newLight = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			memset(newLightBuffer, 0, 100);
			memset(newLightPosition, 0, 12);
			newlightType = 0;
			newLight = false;
		}
		ImGui::End();
	}

	if (newCam)
	{
		ImGui::Begin("New Camera");

		static const char* camtype[] = { "Perspective", "Ortho" };
		static int selectedtype = 0;
		ImGui::Combo("Type", &selectedtype, camtype, 2);
		
		static float fov = 0.0f;
		static float mag = 0.0f;

		if (selectedtype) ImGui::InputFloat("Mag", &mag, 0.1f, 1.0f);
		else ImGui::SliderFloat("FOV", &fov, 1.0f, 180.0f);

		static char nameBuffer[100];
		ImGui::InputText("Name", nameBuffer, 100);

		if (ImGui::Button("Create"))
		{
			std::shared_ptr<Engine::Camera> cam;
			if (selectedtype) cam.reset(new Engine::Camera(mag, float(g_Width) / (float)g_Height, true, -1.0f, 1.0f, nameBuffer));
			else cam.reset(new Engine::Camera(Engine::Util::ToRadian(fov), float(g_Width) / (float)g_Height, 0.1f, 1000.0f, nameBuffer));
			AddCamera(cam);

			mag = 0.0f;
			fov = 0.0f;
			memset(nameBuffer, 0, 100);
			selectedtype = 0;
			newCam = false;

			selectedCamera = int(m_Cameras.size()) - 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			mag = 0.0f;
			fov = 0.0f;
			memset(nameBuffer, 0, 100);
			selectedtype = 0;
			newCam = false;
		}


		ImGui::End();
	}

	if (deleteLight)
	{
		deleteLight = false;
		std::string lightname = m_Lights[selectedLight]->name;
		m_Lights.erase(m_Lights.begin() + selectedLight);
		if (selectedLight == m_Lights.size())
		{
			if(selectedLight)
				--selectedLight;
		}
		LOG_TRACE("On {0} scene deleted light : {1}", m_Name, lightname);
	}

	if (setScript)
	{
		ImGui::Begin("Set Script");
		const char* scriptList[] = { "None", "MultiLighting" };
		static int listidx = 0;
		ImGui::Combo("selected script", &listidx, scriptList, 2);
		if (ImGui::Button("Set"))
		{
			if (listidx != 0)
			{
				SetScript(scriptList[listidx]);
				listidx = 0;
				runScript = false;
				pauseScript = false;
			}
			setScript = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			listidx = 0;
			setScript = false;
		}
		ImGui::End();
	}

}


SceneInform Scene::SaveSceneData()
{
	SceneInform inform;
	inform.SceneName = m_Name;
	inform.CurrentCamIdx = activateCamIdx;
	inform.World = m_worldInform;
	if (m_MyScript)
		inform.ScriptName = m_MyScript->m_Name;
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
			matInform.DiffuseMap = material->MaterialTextures[i][0].Name;
			matInform.NormalMap = material->MaterialTextures[i][1].Name;
			matInform.SpecularMap = material->MaterialTextures[i][2].Name;

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

void Scene::LoadSceneData(const SceneInform& _inform)
{
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
				int animIdx = 1;
				bool findCurAnim = false;

				if (inform.AnimInfo.CurAnimation != "T-Pose")
				{
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
			auto& texture = materialset->MaterialTextures;
			mat.Ambient = inform.Material[i].Ambient;
			mat.Diffuse = inform.Material[i].Diffuse;
			mat.Specular = inform.Material[i].Specular;
			mat.MapMode = inform.Material[i].Mapmode;
			mat.Shiness = inform.Material[i].Shiness;
			texture[i][0].Name = inform.Material[i].DiffuseMap;
			texture[i][1].Name = inform.Material[i].NormalMap;
			texture[i][2].Name = inform.Material[i].SpecularMap;
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
		light->SetType(static_cast<Engine::Light::Type>(inform.type));
		m_Lights.push_back(light);
	}

}

void Scene::SetScript(const std::string & scriptName)
{
	if (scriptName == "MultiLighting")
	{
		m_MyScript.reset(new MultiLightingScript(this));
	}
	else if (scriptName == "None")
	{
		m_MyScript.reset();
	}
	else
	{
		LOG_WARN("Can't find {0} script", scriptName);
	}
}

Scene::Scene(const SceneInform & _inform)
{
	m_Name = _inform.SceneName;
	m_worldInform = _inform.World;
	LoadSceneData(_inform);
	SetScript(_inform.ScriptName);
}