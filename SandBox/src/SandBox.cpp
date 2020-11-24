#include "pceh.h"

#include "SandBox.h"

void SandBox::OnUpdate(float dt)
{
	controlUpdate(dt);

	Engine::Renderer::BeginScene(*perspective, light);

	Engine::Renderer::Enque(objmodel, "DebugWindow");
	Engine::Renderer::Enque(fbxmodel, "DebugWindow");
	Engine::Renderer::ClearDepthStencil();

	Engine::Renderer::Enque(fbxmodel);
	Engine::Renderer::Enque(objmodel);

	Engine::Renderer::Enque(debugwindow);

	Engine::Renderer::EndScene();
}

void SandBox::OnAttach()
{
	auto glovEnv = Engine::Renderer::GetGlobalEnv();
	glovEnv->Ambient.x = 0.3f;
	glovEnv->Ambient.y = 0.3f;
	glovEnv->Ambient.z = 0.3f;

	light.m_Direction.x = 0.0f;
	light.m_Direction.y = 0.0f;
	light.m_Direction.z = 1.0f;
	light.m_Direction.w = 0.0f;

	Engine::TextureArchive::Add("DebugWindow", 1280, 760);
	
	fbxmodel = Engine::Model3D::Create(Engine::RenderingShader::SkeletalMesh)
		.buildFromFBX().SetSkeleton("Kachujin");

	objmodel = Engine::Model3D::Create(Engine::RenderingShader::StaticMesh)
		.buildFromOBJ().SetObject("Tree");

	objmodel->m_Transform.SetTranslate(-1.5f, 0.0f, 0.0f);

	debugwindow = Engine::Model2D::Create(Engine::RenderingShader::TwoDimension)
		.SetTexture("DebugWindow");

	debugwindow->m_Transform.SetTranslate(1.5f, 1.0f, 0.0f);
	debugwindow->m_Transform.SetScale(float(width) / (float)height * 1.0f, 1.0f, 0.0f);

	float filedOfView = 3.141592f / 3.0f;
	perspective.reset(new Engine::Camera(filedOfView, float(width) / (float)height));
}

void SandBox::OnDettach()
{
	
}

void SandBox::OnResize()
{
	perspective.reset(new Engine::Camera(3.141592f / 3.0f, float(width) / (float)height));
}

void SandBox::controlUpdate(float dt)
{
	auto& perspectiveTransform = perspective->GetTransform();
	auto& transform = fbxmodel->m_Transform;

	if (GetAsyncKeyState('W') & 0x8000)
	{
		std::cout << perspectiveTransform.GetTranslate().z << std::endl;
		perspectiveTransform.AddTranslate(0.0f, 0.0f, 0.1f);
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		std::cout << perspectiveTransform.GetTranslate().z << std::endl;
		perspectiveTransform.AddTranslate(0.0f, 0.0f, -0.1f);
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		transform.AddTranslate(0.0f, 0.02f, 0.0f);
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		transform.AddTranslate(0.0f, -0.02f, 0.0f);
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		transform.AddRotate(0.0f, 0.02f, 0.0f);
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		transform.AddRotate(0.0f, -0.02f, 0.0f);
	}

	if (GetAsyncKeyState('N') & 0x8000)
	{
		light.m_Intensity += 0.01f;
	}

	if (GetAsyncKeyState('M') & 0x8000)
	{
		light.m_Intensity -= 0.01f;
		if (light.m_Intensity < 0.0f)
		{
			light.m_Intensity = 0.0f;
		}
	}

	static bool myFlag = true;
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		myFlag = true;
	}
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		myFlag = false;
	}

	static float speed = 0.02f;
	if (GetAsyncKeyState(VK_OEM_4) & 0x8000)
	{
		speed -= 0.0001f;
		if (speed < 0.0001f)
			speed = 0.0001f;
		std::cout << "animation speed : " << speed << "\n";
	}
	if (GetAsyncKeyState(VK_OEM_6) & 0x8000)
	{
		speed += 0.0001f;

		std::cout << "animation speed : " << speed << "\n";
	}

	if (myFlag)
	{
		if (GetAsyncKeyState('Z') & 0x8000)
		{
			fbxmodel->m_Animation->Elapsedtime += speed;
		}
		if (GetAsyncKeyState('X') & 0x8000)
		{
			fbxmodel->m_Animation->Elapsedtime -= speed;
		}
	}
	else
	{
		fbxmodel->m_Animation->Elapsedtime += speed;
	}
	fbxmodel->Update(speed);

}

