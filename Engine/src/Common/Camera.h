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
		//Create Perspective camera
		Camera(float fov, float screenAspect);
		//Create Orthographic camera
		Camera(float magnification, float screenAspect, bool dummy);
		~Camera();

		void OnResize(uint32_t width, uint32_t height);
		void SetFov(float fov);
		void SetMagnification(float magnification);

		Transform& GetTransform();
		const mat4& GetViewMatrix();
		const mat4& GetProjectionMatrix();

	private:
		void recalculateViewMatrix();

	private:
		CameraType m_Type = CameraType::None;

		float m_Fov = 0.0f;
		float m_ScreenAspect = 0.0f;
		float m_Magnification = 0.0f; //only for Orthographic
		bool isChangedView = false;

		Transform m_Transform;
		mat4 m_ViewMatrix;
		mat4 m_ProjectionMatrix;
	};

}
