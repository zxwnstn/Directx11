#include "pch.h"

#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	timer = new QTimer;
	timer->start(0);
	connect(timer, &QTimer::timeout, [this]() {
		Engine::Timestep ts;
		auto fps = 1.0f / ts;
		ui->lineEdit->setText(QString::number(fps));
	});
}

MainWindow::~MainWindow()
{
}

Viewport* MainWindow::GetViewport()
{
	return ui->viewport;
}
