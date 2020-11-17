#include "pceh.h"

#include "SandBox.h"

void SandBox::OnUpdate(float dt)
{
	controlUpdate(dt);

	Engine::Renderer::BeginScene(*perspective, light);
	//Engine::Renderer::Enque(buffer, Engine::RenderingShader::Skinned);
	Engine::Renderer::Enque(model);
	Engine::Renderer::EndScene();
}

void SandBox::OnAttach()
{
	/*float vertices[] = {
		0.5f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
	};
	uint32_t indices[] = {
		0, 2, 1
	};

	buffer = Engine::Renderer::GetShader(Engine::RenderingShader::Skinned)
		.CreateCompotibleBuffer()
		.SetBuffer(vertices, indices, 3);*/

	model = Engine::Model3D::Create(Engine::RenderingShader::Skinned)
		.buildFromFBX().SetSkeleton("lucy");

	//model->m_Material->SetMaterialTexture(Engine::Texture::UsageType::eDiffuse, false);
	//model->m_Material->SetMaterialTexture(Engine::Texture::UsageType::eNormal, true);
	//model->m_Material->SetMaterialTexture(Engine::Texture::UsageType::eSpecular, true);

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
	auto& transform = model->m_Transform;

	if (GetAsyncKeyState('T') & 0x8000)
	{
		model->SetShader(Engine::ToString(Engine::RenderingShader::Skinned));
	}
	if (GetAsyncKeyState('R') & 0x8000)
	{
		model->SetShader(Engine::ToString(Engine::RenderingShader::Lighting));
	}

	if (GetAsyncKeyState('W') & 0x8000)
	{
		std::cout << perspectiveTransform.GetTranslateValue().z << std::endl;
		perspectiveTransform.AddTranslate(0.0f, 0.0f, 0.5f);
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		std::cout << perspectiveTransform.GetTranslateValue().z << std::endl;
		perspectiveTransform.AddTranslate(0.0f, 0.0f, -0.5f);
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

	/*if (GetAsyncKeyState(VK_NUMPAD1) & 0x8000)
	{
		model->m_Material->SetMaterialTexture(Engine::Texture::UsageType::eDiffuse, false);
	}
	if (GetAsyncKeyState(VK_NUMPAD2) & 0x8000)
	{
		model->m_Material->SetMaterialTexture(Engine::Texture::UsageType::eSpecular, false);
	}
	if (GetAsyncKeyState(VK_NUMPAD3) & 0x8000)
	{
		model->m_Material->SetMaterialTexture(Engine::Texture::UsageType::eNormal, false);
	}
	if (GetAsyncKeyState(VK_NUMPAD4) & 0x8000)
	{
		model->m_Material->SetMaterialTexture(Engine::Texture::UsageType::eDiffuse, true);
	}
	if (GetAsyncKeyState(VK_NUMPAD5) & 0x8000)
	{
		model->m_Material->SetMaterialTexture(Engine::Texture::UsageType::eSpecular, true);
	}
	if (GetAsyncKeyState(VK_NUMPAD6) & 0x8000)
	{
		model->m_Material->SetMaterialTexture(Engine::Texture::UsageType::eNormal, true);
	}*/


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

	for (int i = 0; i < 11; ++i)
	{
		if (GetAsyncKeyState('1' + i) & 0x8000)
		{
			model->m_Animation->Expired = false;
			model->m_Animation->Elapsedtime = 0.0f;

			model->SetAnimation(model->m_Animation->AnimList[i], true);
		}
	}

	if (myFlag)
	{
		if (GetAsyncKeyState('Z') & 0x8000)
		{
			model->m_Animation->Elapsedtime += speed;
		}
		if (GetAsyncKeyState('X') & 0x8000)
		{
			model->m_Animation->Elapsedtime -= speed;
		}
	}
	else
	{
		model->m_Animation->Elapsedtime += speed;
	}
	model->Update(speed);

}

