#pragma once

#include "Transform.h"

namespace Engine {

	struct Environment
	{
		DirectX::XMFLOAT4 Ambient{ 0.0f, 0.0f, 0.0f, 1.0f };
	};

	struct Light
	{
		enum Type : unsigned int
		{
			Directional,
			Point,
			Spot
		};
		Type m_Type = Type::Directional;

		Transform m_Transform;
		DirectX::XMFLOAT3 m_Direction{0.0f, 0.0f, 1.0f};
		DirectX::XMFLOAT3 m_Color{1.0f, 1.0f, 1.0f};
	};

}
