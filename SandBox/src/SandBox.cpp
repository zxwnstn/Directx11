#include "pceh.h"

#include "SandBox.h"

void SandBox::OnUpdate(float dt)
{
	controlUpdate(dt);

	Renderer::BeginScene(*perspective);
	Renderer::Enque(model);
	Renderer::EndScene();
}

void SandBox::OnAttach()
{
	model = Model3D::Create(RenderingShader::Skinned)
		.buildFromFBX().SetSkeleton("Kachujin");

	float filedOfView = 3.141592f / 3.0f;
	perspective.reset(new Camera(filedOfView, Window::Prop.Width / (float)Window::Prop.Height));
	ortho.reset(new Camera(Window::Prop.Width / (float)Window::Prop.Height));
}

void SandBox::OnDettach()
{
	
}

void SandBox::controlUpdate(float dt)
{
	auto pos = perspective->GetPosition();
	auto& transform = model->m_Transform;
	if (GetAsyncKeyState('W') & 0x8000)
	{
		pos.z += 0.1f;
		perspective->SetPosition(pos.x, pos.y, pos.z);
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		pos.z -= 0.1f;
		perspective->SetPosition(pos.x, pos.y, pos.z);
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

