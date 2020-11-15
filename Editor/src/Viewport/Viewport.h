#pragma once

#include "RenderingThread.h"

class Viewport : public QWidget
{
	Q_OBJECT
public:
	Viewport(QWidget* parent = nullptr);
	~Viewport();

public:
	void Init();

protected:
	void resizeEvent(QResizeEvent* event) override;

private:
	RenderingThread* m_RenderThread;
};