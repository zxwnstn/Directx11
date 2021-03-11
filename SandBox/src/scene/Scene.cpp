#include "pceh.h"

#include "Scene.h"

void Scene::ChangeInto()
{
	Engine::Renderer::SetRenderingPath(m_RenderingPath);
}
