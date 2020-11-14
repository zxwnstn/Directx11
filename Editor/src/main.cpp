#include "pch.h"

#include "MainWindow/MainWindow.h"
#include "Viewport/Viewport.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	MainWindow mainWindow;
	
	mainWindow.GetViewport()->Init();
	mainWindow.show();

	return app.exec();
}