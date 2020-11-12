#include "pch.h"

#include "SandBox.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Resource/Texture.h"
#include "FbxLoader.h"

void SandBox::OnUpdate(float dt)
{
	auto pos = perspective->GetPosition();

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
	if (GetAsyncKeyState('A') & 0x8000)
	{
		
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		
	}



	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		transform.AddTranslate(0.0f, 0.1f, 0.0f);
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		transform.AddTranslate(0.0f, -0.1f, 0.0f);
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		transform.AddRotate(0.0f, 0.02f, 0.0f);
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		transform.AddRotate(0.0f, -0.02f, 0.0f);
	}

	

	skeleton.Update(dt);
	Renderer::GetDefaultShader(DefaultShader::Skinned).SetBoneParam(skeleton);
	Renderer::GetDefaultShader(DefaultShader::Skinned).SetTransformParam(transform);
	//Renderer::GetDefaultShader(DefaultShader::Color).SetTransformParam(transform);

	//perspective->UpdateViewMatrix();

	Renderer::BeginScene(*perspective);
	//Renderer::Enque(DefaultShader::Color, *TextureBuffer);
	Renderer::Enque(DefaultShader::Skinned, *buffer, *texture);
	//Renderer::Enque(DefaultShader::Color, *buffer);
	//Renderer::Enque(DefaultShader::Color, *buffer2);
	//Renderer::Enque(DefaultShader::Color, *buffer);
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

	FbxResult ret = FBXLoader::Get()->Import("playerWalking.fbx");
	auto size = ret.Indices.size() * 14 * 4;
	skeleton = ret.skeleton;
	
	buffer = Renderer::GetDefaultShader(DefaultShader::Skinned)
		.CreateCompotibleBuffer()
		.SetVertex((float*)ret.Vertices.data(), size)
		.SetIndex(ret.Indices.data(), ret.Indices.size());
	
	texture = std::make_shared<Texture>("playerWalking.fbm/Kachujin_diffuse.png");

	transform.SetRotate(0.0f, 0.0f, 0.0f);

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

	TextureBuffer = Renderer::GetDefaultShader(DefaultShader::Color)
		.CreateCompotibleBuffer()
		.SetVertex(vertices2, sizeof(vertices2))
		.SetIndex(indices2, 36);

	//texture = std::make_shared<Texture>("assets/Texture/stone01.tga");

	float filedOfView = 3.141592f / 3.0f;
	perspective.reset(new Camera(filedOfView, Window::Prop.Width / (float)Window::Prop.Height));
	//ortho.reset(new Camera(Window::Prop.Width / (float)Window::Prop.Height));
}

void SandBox::OnDettach()
{
	
}

