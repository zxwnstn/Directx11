#include "pch.h"

#include "Camera.h"
#include "Util/Math.h"

#define Rad 0.0174532925f

namespace Engine {

	using namespace DirectX;

	Camera::Camera(float fov, float screenAspect, float _near, float _far, const std::string& name)
		: m_Type(CameraType::Perspective), m_Fov(fov), m_ScreenAspect(screenAspect), m_Near(_near), m_Far(_far), m_Cameraname(name)
	{
		m_ProjectionMatrix = Util::GetPerspective(m_Fov, m_ScreenAspect, m_Near, m_Far);
	}

	Camera::Camera(float magnification, float screenAspect, bool dummy, float _near, float _far, const std::string& name)
		: m_Type(CameraType::Ortho), m_Magnification(magnification), m_ScreenAspect(screenAspect), m_Near(_near), m_Far(_far), m_Cameraname(name)
	{
		m_ProjectionMatrix = Util::GetOrthographic(m_Magnification, m_ScreenAspect, m_Near, m_Far);
	}

	Camera::~Camera()
	{
	}

	void Camera::OnResize(uint32_t width, uint32_t height)
	{
		m_ScreenAspect = width / (float)height;	
		recalculateProjectionMatrix();
	}

	void Camera::SetType(CameraType type)
	{
		m_Type = type;
		switch (m_Type)
		{
		case Engine::CameraType::Ortho:
			if (m_Magnification == -1.0f) m_Magnification = 1.0f;
			break;
		case Engine::CameraType::Perspective:
			if (m_Fov == -1.0f) m_Fov = 3.141592f / 3.0f;
			break;
		}
		recalculateProjectionMatrix();
	}

	void Camera::SetFov(float fov)
	{
		if (m_Type != CameraType::Perspective)
			return;

		m_Fov = fov;
		recalculateProjectionMatrix();
	}

	void Camera::SetMagnification(float magnification)
	{
		if (m_Type != CameraType::Ortho)
			return;
		m_Magnification = magnification;
		recalculateProjectionMatrix();
	}

	void Camera::SetNear(float _near)
	{
		m_Near = _near;
		recalculateProjectionMatrix();
	}

	void Camera::SetFar(float _far)
	{
		m_Far = _far;
		recalculateProjectionMatrix();
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

	void Camera::recalculateProjectionMatrix()
	{
		switch (m_Type)
		{
		case Engine::CameraType::Ortho:
			m_ProjectionMatrix = Util::GetOrthographic(m_Magnification, m_ScreenAspect, m_Near, m_Far);
			break;
		case Engine::CameraType::Perspective:
			m_ProjectionMatrix = Util::GetPerspective(m_Fov, m_ScreenAspect, m_Near, m_Far);
			break;
		}
	}
	


}



