#include "pceh.h"

#include "Scene.h"
#include "MultiLighting.h"


void MultiLightingScript::OnStart()
{
	Script::OnStart();
	m_ActivateCam = m_MyScene->GetCurCam();


	m_Green = m_MyScene->GetLight("Main Light");
	m_Magenta = m_MyScene->GetLight("p1");
	m_Red = m_MyScene->GetLight("p2");
	m_Blue = m_MyScene->GetLight("spot1");
	m_Yello = m_MyScene->GetLight("spot2");
	m_Cyan = m_MyScene->GetLight("p3");
	m_Ring = m_MyScene->GetModel3d("ring");

	auto kachu = m_MyScene->GetModel3d("kachu");
	kachu->SetAnimation("playerWalking", true);
	kachu->PlayAnimation();

	auto jamie = m_MyScene->GetModel3d("jamie");
	jamie->SetAnimation("Hip Hop Dancing", true);
	jamie->PlayAnimation();

	auto pearl = m_MyScene->GetModel3d("pearl");
	pearl->SetAnimation("Capoeira", true);
	pearl->PlayAnimation();
}

void MultiLightingScript::OnStop()
{
	Script::OnStop();
}

void MultiLightingScript::OnUpdate(float dt)
{
	//point lights
	m_Green->lightCam.GetTransform().RotateAround(Engine::vec3{ 3.0f, 6.0f, 0.0f },
		Engine::vec3{ 1.0f, 1.0f, 0.0f }, Engine::Util::ToRadian(120.0f * dt));

	m_Magenta->lightCam.GetTransform().RotateAround(Engine::vec3{ -2.0f, 10.0f, 3.0f },
		Engine::vec3{ 0.7f, 1.0f, 1.0f }, Engine::Util::ToRadian(30.0f * dt));

	m_Red->lightCam.GetTransform().RotateAround(Engine::vec3{ 2.0f, 2.0f, 5.0f },
		Engine::vec3{ 0.0f, 1.0f, 0.0f }, Engine::Util::ToRadian(45.0f * dt));

	//spot lights
	m_Yello->lightCam.GetTransform().AddRotate(1.0f * dt, 2.0f* dt, 3.0f * dt);
	m_Blue->lightCam.GetTransform().AddRotate(2.0f * dt, 4.0f * dt, 1.0f * dt);

	m_Ring->m_Transform.AddRotate(0.0f, 3.14f * dt, 1.57 * dt);

	if (cyanForward)
	{
		m_Cyan->lightCam.GetTransform().AddTranslate(4.0f * dt, 0.0f, 4.0f * dt);
		if (m_Cyan->lightCam.GetTransform().GetTranslate().x > 9.0f)
			cyanForward = false;
	}
	else
	{
		m_Cyan->lightCam.GetTransform().AddTranslate(-4.0f * dt, 0.0f, -4.0f * dt);
		if (m_Cyan->lightCam.GetTransform().GetTranslate().x < -9.0f)
			cyanForward = true;
	}

	auto& mtColor = m_Magenta->m_Color;
	if (mtR_Add)
	{
		mtColor.x += 0.2f * dt;
		if (mtColor.x > 0.99f)
		{
			mtR_Add = false;
		}
	}
	else
	{
		mtColor.x -= 0.3f * dt;
		if (mtColor.x < 0.1f)
		{
			mtR_Add = true;
		}
	}

	if (mtG_Add)
	{
		mtColor.y += 0.15f * dt;
		if (mtColor.y > 0.99f)
		{
			mtG_Add = false;
		}
	}
	else
	{
		mtColor.y -= 0.1f * dt;
		if (mtColor.y < 0.1f)
		{
			mtG_Add = true;
		}
	}

	if (mtB_Add)
	{
		mtColor.z += 0.4f * dt;
		if (mtColor.z > 0.99f)
		{
			mtB_Add = false;
		}
	}
	else
	{
		mtColor.z -= 0.2f * dt;
		if (mtColor.z < 0.1f)
		{
			mtB_Add = true;
		}
	}

}

void MultiLightingScript::OnMouseMove(float dx, float dy)
{
	DefaultMouseMove(dx, dy);
}

void MultiLightingScript::OnKeyInput()
{
	DefaultKeyInput();
}

