#include "pceh.h"

#include "SandBox.h"

void SandBox::OnUpdate(float dt)
{
	controlUpdate(dt);

	Engine::Renderer::BeginScene(perspective, { light2, light });
	
	Engine::Renderer::Enque3D(fbxmodel);
	Engine::Renderer::Enque3D(objmodel);
	//Engine::Renderer::Enque2D(debugwindow);

	Engine::Renderer::EndScene();
}

void SandBox::OnAttach()
{
	light.reset(new Engine::Light);
	light->m_Color = { 1.0f, 0.0f, 1.0f, 1.0f };
	light->lightCam.GetTransform().SetTranslate(1.0f, 2.0f, 0.0f);
	light->m_Type = Engine::Light::Type::Point;
	light->m_OuterAngle = 3.141592f / 3;
	light->m_InnerAngle = 3.141592f / 6;
	light->m_Range = 20.0f;
	light->lightCam.GetTransform().SetRotate(0.78f, -1.57f, 0.0f);

	light2.reset(new Engine::Light);
	light2->m_Color = { 0.0, 0.0f, 1.0f, 1.0f };
	light2->lightCam.GetTransform().SetTranslate(-1.0f, 2.0f, 0.0f);
	light2->m_Type = Engine::Light::Type::Spot;
	light2->m_OuterAngle = 3.141592f / 3;
	light2->m_InnerAngle = 3.141592f / 6;
	light2->m_Range = 30.0f;
	light2->lightCam.GetTransform().SetRotate(0.78f, 1.57f, 0.0f);

	fbxmodel = Engine::Model3D::Create()
		.buildFromFBX().SetSkeleton("Pearl");
	fbxmodel->m_Transform.SetScale(0.01f, 0.01f, 0.01f);

	for (auto& mat : fbxmodel->m_MaterialSet->Materials)
	{
		mat.second.Ambient.x = 0.8f;
		mat.second.Ambient.y = 0.8f;
		mat.second.Ambient.z = 0.8f;

		mat.second.Specular.x = 0.5f;
		mat.second.Specular.y = 0.5f;
		mat.second.Specular.z = 0.5f;
	}

	objmodel = Engine::Model3D::Create()
		.buildFromOBJ().SetObject("RoomCube_t1");
	//auto& texture = objmodel->m_MaterialSet->MaterialTextures[0];
	//texture[0].Name = "skyBox";
	//objmodel->m_MaterialSet->Materials[0].MapMode |= 1;
	objmodel->m_Transform.SetScale(10.0f, 10.0f, 10.0f);
	objmodel->m_Transform.SetTranslate(0.0f, 10.0f, 0.0f);

	for (auto& mat : objmodel->m_MaterialSet->Materials)
	{
		mat.second.Ambient.x = 1.0f;
		mat.second.Ambient.y = 1.0f;
		mat.second.Ambient.z = 1.0f;
	}

	floor = Engine::Model3D::Create()
		.buildFromOBJ().SetObject("monkey");

	floor->m_Transform.SetScale(1.0f, 1.0f, 1.0f);
	floor->m_Transform.SetTranslate(0.0f, 1.0f, 0.0f);

	debugwindow = Engine::Model2D::Create()
		.SetTexture("images");
	debugwindow->m_Transform.SetTranslate(0.5f, 0.5f, 0.0f);
	debugwindow->m_Transform.SetScale(0.2f, 0.3f, 0.0f);

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

void SandBox::OnMouseMove(float dx, float dy)
{
	auto& perspectiveTransform = perspective->GetTransform();
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		dx *= mouseSensitive;
		dy *= mouseSensitive;
		perspectiveTransform.LocalRotateX(dx);
		perspectiveTransform.LocalRotateY(dy);
	}
}

void SandBox::controlUpdate(float dt)
{
	auto& perspectiveTransform = perspective->GetTransform();
	auto& fbxtransform = light->lightCam.GetTransform();
	auto& objtransform = floor->m_Transform;

	if (GetAsyncKeyState('W') & 0x8000)
	{
		perspectiveTransform.MoveForwad(0.1f);
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		perspectiveTransform.MoveBack(0.1f);
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		perspectiveTransform.MoveLeft(0.1f);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		perspectiveTransform.MoveRight(0.1f);
	}

	if (GetAsyncKeyState('Q') & 0x8000)
	{
		perspectiveTransform.AddTranslate(0.0f, 0.1f, 0.0f);
	}
	if (GetAsyncKeyState('E') & 0x8000)
	{
		std::cout << perspectiveTransform.GetTranslate().z << std::endl;
		perspectiveTransform.AddTranslate(0.0f, -0.1f, 0.0f);
	}

	static float sensitive = 0.0001f;
	static int ss = 1;

	if (GetAsyncKeyState(VK_F1) & 0x8000)
	{
		sensitive *= 10.0f;
		std::cout << "bias sensitive" << sensitive << '\n';
	}
	if (GetAsyncKeyState(VK_F2) & 0x8000)
	{
		sensitive *= 0.1f;
		std::cout << "bias sensitive" << sensitive << '\n';
	}
	if (GetAsyncKeyState(VK_F3) & 0x8000)
	{
		sensitive += sensitive;
		std::cout << "bias sensitive" << sensitive << '\n';
	}
	if (GetAsyncKeyState(VK_F4) & 0x8000)
	{
		sensitive -= sensitive;
		std::cout << "bias sensitive" << sensitive << '\n';
	}
	
	if (GetAsyncKeyState(VK_F5) & 0x8000)
	{
		Engine::Renderer::AdjustDepthBias(ss);
	}
	if (GetAsyncKeyState(VK_F6) & 0x8000)
	{
		Engine::Renderer::AdjustDepthBias(-ss);
	}
	if (GetAsyncKeyState(VK_F7) & 0x8000)
	{
		Engine::Renderer::AdjustSlopeBias(sensitive);
	}
	if (GetAsyncKeyState(VK_F8) & 0x8000)
	{
		Engine::Renderer::AdjustSlopeBias(-sensitive);
	}


	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		fbxtransform.AddTranslate(0.0f, 0.02f, 0.0f);
	}

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		std::cout << fbxtransform.GetTranslate().y << std::endl;
		fbxtransform.AddTranslate(0.0f, 0.02f, 0.0f);
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		std::cout << fbxtransform.GetTranslate().y << std::endl;
		fbxtransform.AddTranslate(0.0f, -0.02f, 0.0f);
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		fbxtransform.AddTranslate(-0.02f, 0.0f, 0.0f);
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		fbxtransform.AddTranslate(0.02f, -0.0f, 0.0f);
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

