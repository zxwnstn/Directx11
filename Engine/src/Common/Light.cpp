#include "pch.h"

#include "Light.h"
#include "Util/Math.h"

namespace Engine {

	Engine::Environment::Environment()
	{
		WorldMatrix = Util::Identity();
	}

	Light::Light()
		: lightCam(3.141592f / 3.0f, 1280.0f / 720.0f)
	{
		//lightCam.GetTransform().SetTranslate(m_Transform.GetTranslate());
	}

}
