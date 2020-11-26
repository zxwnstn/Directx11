#include "pceh.h"

#include "SandBox.h"

void SandBox::OnUpdate(float dt)
{
	controlUpdate(dt);

	Engine::Renderer::BeginScene(perspective, light);
	
	Engine::Renderer::Enque(fbxmodel);
	Engine::Renderer::Enque(objmodel);
	Engine::Renderer::Enque(floor);
	//Engine::Renderer::Enque(debugwindow);

	Engine::Renderer::EndScene();
}

void SandBox::OnAttach()
{
	auto glovEnv = Engine::Renderer::GetGlobalEnv();
	glovEnv->Ambient.x = 0.3f;
	glovEnv->Ambient.y = 0.3f;
	glovEnv->Ambient.z = 0.3f;

	light.reset(new Engine::Light);

	light->m_Direction.x = 1.0f;
	light->m_Direction.y = -1.0f;
	light->m_Direction.z = 0.0f;
	light->m_Direction.w = 0.0f;

	light->m_Transform.SetTranslate(-50.0f, 50.0f, 0.0f);
	light->lightCam.GetTransform().SetTranslate(0.0f, 5.0f, -20.0f);
	light->lightCam.GetTransform().SetRotate(0.0f, 0.0f, 0.0f);

	fbxmodel = Engine::Model3D::Create(Engine::RenderingShader::SkeletalMesh)
		.buildFromFBX().SetSkeleton("Pearl");
	fbxmodel->m_Transform.SetScale(0.01f, 0.01f, 0.01f);

	for (auto& mat : fbxmodel->m_MaterialSet->Materials)
	{
		mat.second.Ambient.x = 0.8f;
		mat.second.Ambient.y = 0.8f;
		mat.second.Ambient.z = 0.8f;
	}

	objmodel = Engine::Model3D::Create(Engine::RenderingShader::StaticMesh)
		.buildFromOBJ().SetObject("Tree");

	objmodel->m_Transform.SetTranslate(-1.5f, 0.0f, 0.0f);

	for (auto& mat : objmodel->m_MaterialSet->Materials)
	{
		mat.second.Ambient.x = 0.8f;
		mat.second.Ambient.y = 0.8f;
		mat.second.Ambient.z = 0.8f;
	}

	floor = Engine::Model2D::Create(Engine::RenderingShader::TwoDimension)
		.SetTexture("stone01");

	floor->m_Transform.SetScale(30.0f, 30.0f, 0.0f);
	floor->m_Transform.SetRotate(1.57f, 0.0f, 0.0f);

	debugwindow = Engine::Model2D::Create(Engine::RenderingShader::TwoDimension)
		.SetTexture("SceneShadow");
	
	debugwindow->m_Transform.SetTranslate(1.0f, 2.0f, 0.0f);
	debugwindow->m_Transform.SetScale(2.0f, 2.0f, 0.0f);

	float filedOfView = 3.141592f / 3.0f;
	perspective.reset(new Engine::Camera(filedOfView, float(width) / (float)height));
	perspective->GetTransform().SetTranslate(0.0f, 2.0f, -4.0f);
}

void SandBox::OnDettach()
{
}

void SandBox::OnResize()
{
	perspective->OnResize(width, height);
}

void SandBox::controlUpdate(float dt)
{
	auto& perspectiveTransform = perspective->GetTransform();
	auto& fbxtransform = fbxmodel->m_Transform;
	auto& objtransform = objmodel->m_Transform;

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

	auto& bias = Engine::Renderer::GetGlobalEnv()->bias;
	if (GetAsyncKeyState('1') & 0x8000)
	{
		bias.x += 0.001f;
		std::cout << "cur shadow bias : " << bias.x << "\n";
	}

	if (GetAsyncKeyState('2') & 0x8000)
	{
		bias.x += 0.0001f;
		std::cout << "cur shadow bias : " << bias.x << "\n";
	}
	if (GetAsyncKeyState('3') & 0x8000)
	{
		bias.x += 0.00001f;
		std::cout << "cur shadow bias : " << bias.x << "\n";
	}
	if (GetAsyncKeyState('4') & 0x8000)
	{
		bias.x -= 0.001f;
		std::cout << "cur shadow bias : " << bias.x << "\n";
	}
	if (GetAsyncKeyState('5') & 0x8000)
	{
		bias.x -= 0.0001f;
		std::cout << "cur shadow bias : " << bias.x << "\n";
	}
	if (GetAsyncKeyState('6') & 0x8000)
	{
		bias.x -= 0.00001f;
		std::cout << "cur shadow bias : " << bias.x << "\n";
	}

	if (GetAsyncKeyState(VK_NUMPAD1) & 0x8000)
	{
		bias.y += 0.001f;
		std::cout << "cur clamp bias y: " << bias.y << "\n";
	}
	if (GetAsyncKeyState(VK_NUMPAD2) & 0x8000)
	{
		bias.z += 0.001f;
		std::cout << "cur clamp bias z: " << bias.z << "\n";
	}

	if (GetAsyncKeyState(VK_NUMPAD4) & 0x8000)
	{
		bias.y -= 0.001f;
		std::cout << "cur clamp bias y: " << bias.y << "\n";
	}

	if (GetAsyncKeyState(VK_NUMPAD5) & 0x8000)
	{
		bias.z -= 0.001f;
		std::cout << "cur clamp bias z: " << bias.z << "\n";
	}

	if (GetAsyncKeyState(VK_F1) & 0x8000)
	{
		objtransform.AddTranslate(0.0f, 0.02f, 0.0f);
	}
	if (GetAsyncKeyState(VK_F2) & 0x8000)
	{
		objtransform.AddTranslate(0.0f, -0.02f, 0.0f);
	}

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		fbxtransform.AddTranslate(0.0f, 0.02f, 0.0f);
	}

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		fbxtransform.AddTranslate(0.0f, 0.02f, 0.0f);
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		fbxtransform.AddTranslate(0.0f, -0.02f, 0.0f);
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		fbxtransform.AddRotate(0.0f, 0.02f, 0.0f);
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		fbxtransform.AddRotate(0.0f, -0.02f, 0.0f);
	}

	if (GetAsyncKeyState('N') & 0x8000)
	{
		light->m_Intensity += 0.01f;
	}

	if (GetAsyncKeyState('M') & 0x8000)
	{
		light->m_Intensity -= 0.01f;
		if (light->m_Intensity < 0.0f)
		{
			light->m_Intensity = 0.0f;
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

