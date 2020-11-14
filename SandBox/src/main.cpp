#include "pceh.h"

#include "SandBox.h"

int main()
{
	App app(WindowProp{ L"Dx11Study", 1280, 760});
	app.PushLayer(new SandBox, "SandBox");


	return app.Run();
}

