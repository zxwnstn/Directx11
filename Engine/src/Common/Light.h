#pragma once

#include "Camera.h"

namespace Engine {

	struct Environment
	{
		vec3 Ambient{ 0.0f, 0.0f, 0.0f};
		mat4 WorldMatrix;
		bool UseShadowMap = false;
		vec4 bias;
		Environment();
	};

	struct Light
	{
		Light();
		enum Type : unsigned int
		{
			Directional,
			Point,
			Spot
		};
		Transform m_Transform;
		Camera lightCam;
		vec4 m_Direction{0.0f, 0.0f, 1.0f, 0.0f};
		vec4 m_Color{1.0f, 1.0f, 1.0f, 1.0f};
		Type m_Type = Type::Directional;
		float m_Intensity = 1;
	};

}
