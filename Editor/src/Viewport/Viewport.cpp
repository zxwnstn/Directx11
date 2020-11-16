#include "pch.h"

#include "Viewport.h"

Viewport::Viewport(QWidget* parent)
	: QWidget(parent)
{	
	QWidget::setFocusPolicy(Qt::StrongFocus);

	this->setAutoFillBackground(false);
}

Viewport::~Viewport()
{
}

void Viewport::Init()
{
	Engine::ModuleCore::Init({ width(), height(), winId() });
	m_RenderThread = new RenderingThread;

	m_RenderThread->w = width();
	m_RenderThread->h = height();
	m_RenderThread->resized = true;

	m_RenderThread->start();
}

void Viewport::resizeEvent(QResizeEvent * event)
{
	m_RenderThread->h = height();
	m_RenderThread->w = width();
	m_RenderThread->resized = true;

	event->accept();
}

