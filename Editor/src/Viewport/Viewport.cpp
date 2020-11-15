#include "pch.h"

#include "Viewport.h"

std::shared_ptr<Engine::Model3D> model;
std::shared_ptr<Engine::Camera> perspective;

Viewport::Viewport(QWidget* parent)
	: QWidget(parent)
{	
	QWidget::setFocusPolicy(Qt::StrongFocus);


	this->setAutoFillBackground(false);
}

Viewport::~Viewport()
{
}

bool resized = false;
uint32_t w;
uint32_t h;

void work()
{
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

		model->Update(ts);
		ts.Update();

		static size_t i = 0;
		i++;
		Engine::Renderer::BeginScene(*perspective);
		Engine::Renderer::Enque(model);
		Engine::Renderer::EndScene();

		std::cout << i << "on Render!\n";
	}
}

void Viewport::Init()
{
	Engine::ModuleCore::Init({ width(), height(), winId() });
	model = Engine::Model3D::Create(Engine::RenderingShader::Skinned)
		.buildFromFBX().SetSkeleton("Kachujin");

	float filedOfView = 3.141592f / 3.0f;
	perspective.reset(new Engine::Camera(filedOfView, width() / (float)height()));

	this->setFocus();

	std::thread t(work);
	t.detach();

	Engine::Timestep::SetTimePoint();
	//timer = new QTimer;
	//timer->start(0);
	//connect(timer, &QTimer::timeout, [this]() { Update(); Render();});
}

void Viewport::Update()
{
	/*Engine::Timestep ts;
	model->Update(ts);
	ts.Update();*/
}

void Viewport::Render()
{
	static size_t i = 0;
	/*i++;
	Engine::Renderer::BeginScene(*perspective);
	Engine::Renderer::Enque(model);
	Engine::Renderer::EndScene();*/

	std::cout << i << "on Render!\n";
}

void Viewport::keyPressEvent(QKeyEvent * event)
{
	auto pos = perspective->GetPosition();
	auto& transform = model->m_Transform;
	if (event->key() == Qt::Key::Key_W)
	{
		pos.z += 0.1f;
		perspective->SetPosition(pos.x, pos.y, pos.z);
	}
	if (event->key() == Qt::Key::Key_S)
	{
		pos.z -= 0.1f;
		perspective->SetPosition(pos.x, pos.y, pos.z);
	}

	if (event->key() == Qt::Key::Key_Up)
	{
		transform.AddTranslate(0.0f, 0.01f, 0.0f);
	}
	if (event->key() == Qt::Key::Key_Down)
	{
		transform.AddTranslate(0.0f, -0.01f, 0.0f);
	}
	if (event->key() == Qt::Key::Key_Left)
	{
		transform.AddTranslate(-0.01f, 0.0f, 0.0f);
	}
	if (event->key() == Qt::Key::Key_Right)
	{
		transform.AddTranslate(0.01f, 0.0f, 0.0f);
	}
	event->accept();
}

void Viewport::resizeEvent(QResizeEvent * event)
{
	/*uint32_t width = (uint32_t)event->size().width();
	uint32_t height = (uint32_t)event->size().height();
	Engine::Renderer::Resize(width, height);
	perspective->Resize(width, height);*/
	
	w = width();
	h = height();
	resized = true;

	event->accept();
}

void Viewport::paintEvent(QPaintEvent * event)
{
	
}

