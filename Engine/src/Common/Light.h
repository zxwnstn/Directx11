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
		Camera lightCam;
		vec4 m_Color{1.0f, 1.0f, 1.0f, 1.0f};
		Type m_Type = Type::Directional;
		float m_InnerAngle = 0.0f;
		float m_OuterAngle = 0.0f;
		float m_Intensity = 1;
		float m_Range = 10.0f;
	};

}
