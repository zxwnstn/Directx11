#include "pch.h"

#include "Camera.h"

#define Rad 0.0174532925f

namespace Engine {

	using namespace DirectX;

	Camera::Camera(float fov, float screenAspect)
	{
		m_Type = CameraType::Perspective;

		m_Fov = fov;
		m_ScreenAspect = screenAspect;
		m_ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fov, m_ScreenAspect, 0.1f, 10000.0f);
		m_Transform.SetTranslate(0.0f, 0.0f, -1.0f);
	}

	Camera::Camera(float screenAspect)
	{
		m_Type = CameraType::Ortho;

		m_ScreenAspect = screenAspect;
		m_ProjectionMatrix = DirectX::XMMatrixOrthographicLH(screenAspect * 5.0f, 5.0f, 30.f, 10000.0f);
		m_Transform.SetTranslate(0.0f, 0.0f, -1.0f);
	}

	Camera::~Camera()
	{
	}

	void Camera::Resize(uint32_t width, uint32_t height)
	{
		m_ScreenAspect = width / (float)height;
		switch (m_Type)
		{
		case Engine::CameraType::Ortho:
			m_ProjectionMatrix = DirectX::XMMatrixOrthographicLH(m_ScreenAspect * 5.0f, 5.0f, 30.f, 10000.0f);
			break;
		case Engine::CameraType::Perspective:
			m_ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(m_Fov, m_ScreenAspect, 0.1f, 10000.0f);
			break;
		}
	}

	void Camera::recalculateViewMatrix()
	{
		DirectX::XMFLOAT3 up, position, lookAt;
		DirectX::XMVECTOR upVector, positionVector, lookAtVector;
		DirectX::XMMATRIX rotationMatrix;

		//position vector
		position = m_Transform.GetTranslateValue();
		positionVector = DirectX::XMLoadFloat3(&position);

		//look at vector
		lookAt.x = 0.0f;
		lookAt.y = 0.0f;
		lookAt.z = 1.0f;
		lookAtVector = XMLoadFloat3(&lookAt);
		rotationMatrix = m_Transform.GetRotate();
		lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
		lookAtVector = DirectX::XMVectorAdd(positionVector, lookAtVector);

		//up vector
		up.x = 0.0f;
		up.y = 1.0f;
		up.z = 0.0f;
		upVector = XMLoadFloat3(&up);
		upVector = XMVector3TransformCoord(upVector, rotationMatrix);

		m_ViewMatrix = DirectX::XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
	}

	const DirectX::XMMATRIX & Camera::GetViewMatrix()
	{
		recalculateViewMatrix();
		return m_ViewMatrix;
	}

	const DirectX::XMMATRIX & Camera::GetProjectionMatrix()
	{
		return m_ProjectionMatrix;
	}

}



