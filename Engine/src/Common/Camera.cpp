#include "pch.h"

#include "Camera.h"
#include "Util/Math.h"

#define Rad 0.0174532925f

namespace Engine {

	using namespace DirectX;

	Camera::Camera(float fov, float screenAspect)
		: m_Type(CameraType::Perspective), m_Fov(fov), m_ScreenAspect(screenAspect)
	{
		m_ProjectionMatrix = Util::GetPerspective(m_Fov, m_ScreenAspect, 0.1f, 1000.0f);
	}

	Camera::Camera(float magnification, float screenAspect, bool dummy)
		: m_Type(CameraType::Ortho), m_Magnification(magnification), m_ScreenAspect(screenAspect)
	{
		m_ProjectionMatrix = Util::GetOrthographic(m_Magnification, m_ScreenAspect, -1.0f, 1.0f);
	}

	Camera::~Camera()
	{
	}

	void Camera::OnResize(uint32_t width, uint32_t height)
	{
		m_ScreenAspect = width / (float)height;
		switch (m_Type)
		{
		case Engine::CameraType::Ortho:
			m_ProjectionMatrix = Util::GetOrthographic(m_Magnification, m_ScreenAspect, -1.0f, 1.0f);
			break;
		case Engine::CameraType::Perspective:
			m_ProjectionMatrix = Util::GetPerspective(m_Fov, m_ScreenAspect, 0.1f, 1000.0f);
			break;
		}
	}

	void Camera::SetFov(float fov)
	{
		if (m_Type != CameraType::Perspective)
			return;

		m_Fov = fov;
		m_ProjectionMatrix = Util::GetPerspective(m_Fov, m_ScreenAspect, 0.1f, 1000.0f);
	}

	void Camera::SetMagnification(float magnification)
	{
		if (m_Type != CameraType::Ortho)
			return;
		m_Magnification = magnification;
		m_ProjectionMatrix = Util::GetOrthographic(m_Magnification, m_ScreenAspect, -1.0f, 1.0f);
	}

	Transform& Camera::GetTransform() 
	{ 
		isChangedView = true; 
		return m_Transform; 
	}

	const mat4& Camera::GetViewMatrix() 
	{ 
		recalculateViewMatrix(); 
		return m_ViewMatrix; 
	}

	const mat4& Camera::GetProjectionMatrix() 
	{ 
		recalculateViewMatrix(); 
		return m_ProjectionMatrix; 
	}

	void Camera::recalculateViewMatrix()
	{	
		if (!isChangedView)
			return;
		m_ViewMatrix = Util::GetViewMatrix(m_Transform.GetTranslate(), m_Transform.GetRotate());
		isChangedView = false;
	}


}



