#pragma once

#include "Transform.h"

namespace Engine {

	struct Environment
	{
		vec3 Ambient{ 0.0f, 0.0f, 0.0f};
		mat4 WorldMatrix;
		Environment();
	};

	struct Light
	{
		enum Type : unsigned int
		{
			Directional,
			Point,
			Spot
		};
		Transform m_Transform;
		vec4 m_Direction{0.0f, 0.0f, 1.0f, 0.0f};
		vec4 m_Color{1.0f, 1.0f, 1.0f, 1.0f};
		Type m_Type = Type::Directional;
		float m_Intensity = 1;
	};

}
