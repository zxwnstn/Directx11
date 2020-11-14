#include "pch.h"

#include "Viewport.h"

Viewport::Viewport(QWidget* parent)
	: QWidget(parent)
{	
	
}

Viewport::~Viewport()
{
}

void Viewport::Init()
{
	Engine::ModuleCore::Init({ width(), height(), winId() });
	model = Engine::Model3D::Create(Engine::RenderingShader::Skinned)
		.buildFromFBX().SetSkeleton("Kachujin");

	float filedOfView = 3.141592f / 3.0f;
	perspective.reset(new Engine::Camera(filedOfView, width() / (float)height()));

	timer = new QTimer;
	timer->start(0);

	Engine::Timestep::SetTimePoint();
	connect(timer, &QTimer::timeout, [this]() { Update(); Render();});
}

void Viewport::Update()
{
	Engine::Timestep ts;
	model->Update(ts);
	ts.Update();
}

void Viewport::Render()
{
	Engine::Renderer::BeginScene(*perspective);
	Engine::Renderer::Enque(model);
	Engine::Renderer::EndScene();
}

void Viewport::keyPressEvent(QKeyEvent * event)
{
}

