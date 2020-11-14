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
	ModuleCore::Init({ width(), height(), winId() });
	model = Model3D::Create(RenderingShader::Skinned)
		.buildFromFBX().SetSkeleton("Kachujin");

	float filedOfView = 3.141592f / 3.0f;
	perspective.reset(new Camera(filedOfView, width() / (float)height()));

	timer = new QTimer;
	timer->start(0);

	Timestep::SetTimePoint();
	connect(timer, &QTimer::timeout, [this]() { Update(); Render();});
}

void Viewport::Update()
{
	Timestep ts;
	model->Update(ts);
	ts.Update();
}

void Viewport::Render()
{
	Renderer::BeginScene(*perspective);
	Renderer::Enque(model);
	Renderer::EndScene();
}

void Viewport::keyPressEvent(QKeyEvent * event)
{
}

