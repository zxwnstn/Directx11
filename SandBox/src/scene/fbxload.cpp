#include "pceh.h"

#include "fbxload.h"
#include "../../vendor/imgui/imgui.h"

void FbxLoadScene::Init()
{
	m_Name = "FBX file load";
	m_RenderingPath = Engine::RenderingPath::Fbx;

	float filedOfView = 3.141592f / 3.0f;
	m_Cur_cam.reset(new Engine::Camera(filedOfView, float(width) / (float)height));
	m_Cur_cam->GetTransform().SetTranslate(0.0f, 2.0f, 4.0f);
	m_Cur_cam->GetTransform().SetRotate(0.0f, 3.14f, 0.0f);

	m_Cameras.push_back(m_Cur_cam);

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
}

void FbxLoadScene::OnUpdate(float dt)
{
	m_Model3[curModelIdx]->Update(animationSpeed);
	std::cout << dt << std::endl;
	Engine::Renderer::Enque3D(m_Model3[curModelIdx]);
}

void FbxLoadScene::OnImGui()
{
	auto animlist = Engine::SkeletalAnimationArchive::GetAnimList(ModelNames[curModelIdx]);

	const char* arr[10];
	const char* arr2[10];
	for (int i = 0; i < ModelNames.size(); ++i)
		arr[i] = ModelNames[i].c_str();
	for (int i = 0; i < animlist.size(); ++i)
		arr2[i + 1] = animlist[i].c_str();
	arr2[0] = "T-Pose";

	ImGui::Begin("Fbx");
	ImGui::Combo("", &curModelIdx, arr, ModelNames.size());
	if(ImGui::CollapsingHeader("Detail"))
	{
		if (ImGui::Combo("Animation", &curAnimtionIdx[ModelNames[curModelIdx]], arr2, animlist.size() + 1))
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
	}

	ImGui::End();
}

