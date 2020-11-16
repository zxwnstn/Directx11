#include "pch.h"

#include "RenderingThread.h"

RenderingThread::RenderingThread()
{
	model = Engine::Model3D::Create(Engine::RenderingShader::Lighting)
		.buildFromFBX().SetSkeleton("Kachujin");

	float filedOfView = 3.141592f / 3.0f;
	perspective.reset(new Engine::Camera(filedOfView, w / (float)h));
}

void RenderingThread::run()
{
	Engine::Timestep::SetTimePoint();
	while (1)
	{
		Engine::Timestep ts;
		if (ts < 0.016f) continue;

		if (resized)
		{
			Engine::Renderer::Resize(w, h);
			perspective->Resize(w, h);
			resized = false;
		}

		UpdateControl();

		model->Update(speed);
		ts.Update();

		Engine::Renderer::BeginScene(*perspective, light);
		Engine::Renderer::Enque(model);
		Engine::Renderer::EndScene();
	}
}

void RenderingThread::UpdateControl()
{
	if (GetAsyncKeyState('W') & 0x8000)
	{
		perspective->GetTransform().AddTranslate(0.0f, 0.0f, 0.1f);
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		perspective->GetTransform().AddTranslate(0.0f, 0.0f, -0.1f);
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		perspective->GetTransform().AddTranslate(-0.1f, 0.0f, 0.0f);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		perspective->GetTransform().AddTranslate(0.1f, 0.0f, 0.0f);
	}


	if (GetAsyncKeyState('Q') & 0x8000)
	{
		perspective->GetTransform().AddRotate(0.0f, 0.0f, -0.1f);
	}
	if (GetAsyncKeyState('E') & 0x8000)
	{
		perspective->GetTransform().AddRotate(0.0f, 0.0f, 0.1f);
	}
	if (GetAsyncKeyState('Z') & 0x8000)
	{
		perspective->GetTransform().AddRotate(0.0f, -0.1f, 0.0f);
	}
	if (GetAsyncKeyState('C') & 0x8000)
	{
		perspective->GetTransform().AddRotate(0.0f, 0.1f, 0.0f);
	}


	auto& transform = model->m_Transform;
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		transform.MoveForwad(0.1f);
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		transform.MoveBack(0.1f);
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		transform.AddRotate(0.0f, 0.02f, 0.0f);
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		transform.AddRotate(0.0f, -0.02f, 0.0f);
	}


	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		myFlag = true;
	}
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		myFlag = false;
	}

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
		if (GetAsyncKeyState('O') & 0x8000)
		{
			model->m_Animation->Elapsedtime += speed;
		}
		if (GetAsyncKeyState('P') & 0x8000)
		{
			model->m_Animation->Elapsedtime -= speed;
		}
	}
	else
	{
		model->m_Animation->Elapsedtime += speed;
	}
}
