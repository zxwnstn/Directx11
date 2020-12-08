#include "pch.h"

#include "Light.h"
#include "Util/Math.h"

namespace Engine {

	Engine::Environment::Environment()
	{
		WorldMatrix = Util::Identity();
	}

	Light::Light()
		: lightCam(3.141592f / 2.0f, 1.0f)
	{
	}
}
