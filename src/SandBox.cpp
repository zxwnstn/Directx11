#include "pch.h"

#include "SandBox.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"

void SandBox::OnUpdate(float dt)
{
	/*auto pos = perspective->GetPosition();

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
*/
	Renderer::BeginScene(*perspective);
	
	Renderer::EndScene();
}

void SandBox::OnAttach()
{
	/*float vertices[] = {
		0.0f,   0.25f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.5f,
	   -0.25f, -0.25f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.5f,
		0.25f, -0.25f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.5f,
	};

	uint32_t indices[] = {
		0, 2, 1
	};
	std::cout << sizeof(vertices);
	buffer2 = Renderer::GetDefaultShader(DefaultShader::Color)
		.CreateCompotibleBuffer()
		.SetVertex(vertices, sizeof(vertices))
		.SetIndex(indices, 3);*/

	float vertices2[] = {
	   -0.5,  0.5,  0.5, 1.0, 1.0, 1.0, 0.0, 1.0,
	   -0.5, -0.5,  0.5, 1.0, 1.0, 1.0, 0.0, 1.0,
		0.5, -0.5,  0.5, 1.0, 1.0, 1.0, 0.0, 1.0,
		0.5,  0.5,  0.5, 1.0, 1.0, 1.0, 0.0, 1.0,
	   -0.5,  0.5, -0.5, 1.0, 1.0, 1.0, 0.0, 1.0,
	   -0.5, -0.5, -0.5, 1.0, 1.0, 1.0, 0.0, 1.0,
		0.5, -0.5, -0.5, 1.0, 1.0, 1.0, 0.0, 1.0,
		0.5,  0.5, -0.5, 1.0, 1.0, 1.0, 0.0, 1.0,
	};

	uint32_t indices2[] = {
		0, 2, 1, 0, 3, 2,
		0, 4, 3, 4, 7, 3, 
		1, 5, 0, 5, 4, 0,
		3, 6, 2, 3, 7, 6,
		4, 6, 7, 4, 5, 6,
		2, 5, 1, 2, 6, 5,
	};
	//texture = std::make_shared<Texture>("assets/Texture/stone01.tga");

	TextureBuffer = Renderer::GetShader(RenderingShader::Color)
		.CreateCompotibleBuffer()
		.SetVertex(vertices2, sizeof(vertices2))
		.SetIndex(indices2, 36);


	float filedOfView = 3.141592f / 3.0f;
	perspective.reset(new Camera(filedOfView, Window::Prop.Width / (float)Window::Prop.Height));
}

void SandBox::OnDettach()
{
	
}

