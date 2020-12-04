#include "pceh.h"

#include "SandBox.h"

void SandBox::OnUpdate(float dt)
{
	controlUpdate(dt);

	objmodel->m_Transform.SetTranslate(light->lightCam.GetTransform().GetTranslate());
	Engine::Renderer::BeginScene(perspective, light, light2);
	
	Engine::Renderer::Enque(fbxmodel);
	Engine::Renderer::Enque(objmodel);
	Engine::Renderer::Enque(floor);
	//Engine::Renderer::Enque(debugwindow);

	Engine::Renderer::EndScene();
}

void SandBox::OnAttach()
{
	setStaticSqaure();
	auto glovEnv = Engine::Renderer::GetGlobalEnv();
	glovEnv->Ambient.x = 0.3f;
	glovEnv->Ambient.y = 0.3f;
	glovEnv->Ambient.z = 0.3f;

	light.reset(new Engine::Light);
	light->m_Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	light->lightCam.GetTransform().SetTranslate(-1.0f, 1.0f, 0.0f);
	light->m_Type = Engine::Light::Type::Spot;
	light->m_OuterAngle = 3.141592f / 3;
	light->m_InnerAngle = 3.141592f / 6;
	light->m_Range = 30.0f;
	light->lightCam.GetTransform().SetRotate(0.78f, 1.57f, 0.0f);

	light2.reset(new Engine::Light);
	light2->m_Color = { 1.0f, 0.0f, 1.0f, 1.0f };
	light2->lightCam.GetTransform().SetTranslate(3.0f, 1.0f, 0.0f);
	light2->m_Type = Engine::Light::Type::Point;
	light2->m_OuterAngle = 3.141592f / 3;
	light2->m_InnerAngle = 3.141592f / 6;
	light2->m_Range = 30.0f;
	light2->lightCam.GetTransform().SetRotate(0.78f, 1.57f, 0.0f);


	fbxmodel = Engine::Model3D::Create(Engine::RenderingShader::SkeletalDiffered)
		.buildFromFBX().SetSkeleton("Pearl");
	fbxmodel->m_Transform.SetScale(0.01f, 0.01f, 0.01f);

	for (auto& mat : fbxmodel->m_MaterialSet->Materials)
	{
		mat.second.Ambient.x = 0.8f;
		mat.second.Ambient.y = 0.8f;
		mat.second.Ambient.z = 0.8f;
	}

	objmodel = Engine::Model3D::Create(Engine::RenderingShader::StaticDiffered)
		.buildFromOBJ().SetObject("sphere");

	objmodel->m_Transform.SetScale(0.1f, 0.1f, 0.1f);

	for (auto& mat : objmodel->m_MaterialSet->Materials)
	{
		mat.second.Ambient.x = 1.0f;
		mat.second.Ambient.y = 1.0f;
		mat.second.Ambient.z = 1.0f;
	}

	floor = Engine::Model3D::Create(Engine::RenderingShader::StaticDiffered)
		.buildCustum()
		.SetMesh("StaticSquare").SetMaterial("default")
		.Finish();

	floor->m_Transform.SetScale(10.0f, 10.0f, 1.0f);
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

void SandBox::OnMouseMove(float dx, float dy)
{
	auto& perspectiveTransform = perspective->GetTransform();
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		std::cout << "On mouse move " << dx << " " << dy << "\n";
		dx *= mouseSensitive;
		dy *= mouseSensitive;
		perspectiveTransform.LocalRotateX(dx);
		perspectiveTransform.LocalRotateY(dy);

		std::cout << perspectiveTransform.GetRotate().x << " " << perspectiveTransform.GetRotate().y << " " << perspectiveTransform.GetRotate().z << "\n";
	}
}

void SandBox::setStaticSqaure()
{
	auto staticSqare = Engine::MeshArchive::AddStaticMesh("StaticSquare");
	auto& ssvertices = staticSqare->Vertices;
	ssvertices.resize(4);

	ssvertices[0].Position = { -1.0f,  1.0f, 0.0f };
	ssvertices[1].Position = { -1.0f, -1.0f, 0.0f };
	ssvertices[2].Position = { 1.0f, -1.0f, 0.0f };
	ssvertices[3].Position = { 1.0f,  1.0f, 0.0f };
	ssvertices[0].UV = { 0.0f, 0.0f };
	ssvertices[1].UV = { 0.0f, 1.0f };
	ssvertices[2].UV = { 1.0f, 1.0f };
	ssvertices[3].UV = { 1.0f, 0.0f };
	ssvertices[0].Normal = { 0.0f, 0.0f, -1.0f };
	ssvertices[1].Normal = { 0.0f, 0.0f, -1.0f };
	ssvertices[2].Normal = { 0.0f, 0.0f, -1.0f };
	ssvertices[3].Normal = { 0.0f, 0.0f, -1.0f };

	auto[tan, binormal] = Engine::Util::GetTangentAndBinomal(ssvertices[0].Position, ssvertices[1].Position, ssvertices[2].Position,
		ssvertices[0].UV, ssvertices[1].UV, ssvertices[2].UV);
	ssvertices[0].Tangent = tan;
	ssvertices[1].Tangent = tan;
	ssvertices[2].Tangent = tan;
	ssvertices[3].Tangent = tan;
	ssvertices[0].BiNormal = binormal;
	ssvertices[1].BiNormal = binormal;
	ssvertices[2].BiNormal = binormal;
	ssvertices[3].BiNormal = binormal;

	static uint32_t indices[] = {
		0, 2, 1, 0, 3, 2
	};

	staticSqare->Indices = indices;
	staticSqare->IndiceCount = 6;

	auto defaultMat = Engine::MaterialArchive::AddSet("default");
	defaultMat->Materials[0].Ambient = {0.0f, 0.0f, 0.0f, 1.0f};
	defaultMat->Materials[0].Diffuse = {1.0f, 1.0f, 1.0f, 1.0f};
	defaultMat->Materials[0].Specular = {0.5f, 0.5f, 0.5f, 1.0f};
	defaultMat->Materials[0].Shiness = 20.0f;
	defaultMat->Materials[0].MapMode = 0;
}

void SandBox::controlUpdate(float dt)
{
	auto& perspectiveTransform = perspective->GetTransform();
	auto& fbxtransform = light->lightCam.GetTransform();
	auto& objtransform = fbxmodel->m_Transform;

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
		objtransform.AddRotate(0.0f, 0.02f, 0.0f);
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		objtransform.AddRotate(0.0f, -0.02f, 0.0f);
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

