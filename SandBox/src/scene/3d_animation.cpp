#include "pceh.h"

#include "3d_animation.h"
#include "../../vendor/imgui/imgui.h"

void Animation3D::OnUpdate(float dt)
{
}

void Animation3D::OnImGui()
{
	ImGui::Begin("3d animation");
	ImGui::End();
}

void Animation3D::Init()
{
	m_Name = "3D animation play";
	m_RenderingPath = Engine::RenderingPath::Animation;

	float filedOfView = 3.141592f / 3.0f;
	m_Cur_cam.reset(new Engine::Camera(filedOfView, float(width) / (float)height));
	m_Cur_cam->GetTransform().SetTranslate(0.0f, 2.0f, -4.0f);
	m_Cameras.push_back(m_Cur_cam);
}
