#pragma once

#include "Transform.h"

namespace Engine {

	enum class CameraType
	{
		Ortho,
		Perspective,
		None
	};

	class Camera
	{
	public:
		Camera(float fov, float screenAspect);
		Camera(float screenAspect);
		~Camera();

		void Resize(uint32_t width, uint32_t height);
		Transform& GetTransform() { return m_Transform; }

		const DirectX::XMMATRIX& GetViewMatrix();
		const DirectX::XMMATRIX& GetProjectionMatrix();

	private:
		void recalculateViewMatrix();

	private:
		CameraType m_Type = CameraType::None;

		float m_Fov;
		float m_ScreenAspect;
		bool isChangedView = false;

		Transform m_Transform;
		DirectX::XMMATRIX m_ViewMatrix;
		DirectX::XMMATRIX m_ProjectionMatrix;
	};

}
