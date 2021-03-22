#include "pceh.h"

#include "Script.h"

#include "Scene.h"

static SceneInform s_SceneBackUpData;

Script::Script(Scene* myScene, const std::string& name)
	: m_MyScene(myScene), m_Name(name)
{}

void Script::OnStart()
{
	s_SceneBackUpData = m_MyScene->SaveSceneData();
}

void Script::OnStop()
{
	m_MyScene->m_worldInform = s_SceneBackUpData.World;

	m_MyScene->m_Cameras.clear();
	m_MyScene->m_Model3.clear();
	m_MyScene->m_Lights.clear();

	m_MyScene->LoadSceneData(s_SceneBackUpData);
}

void Script::DefaultMouseMove(float dx, float dy)
{
	auto& camTransform = m_MyScene->GetCurCam()->GetTransform();
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		camTransform.LocalRotateX(dx);
		camTransform.LocalRotateY(dy);
	}
}

void Script::DefaultKeyInput()
{
	auto& camTransform = m_MyScene->GetCurCam()->GetTransform();
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

