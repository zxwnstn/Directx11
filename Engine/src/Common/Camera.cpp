#include "pch.h"

#include "Camera.h"

#define Rad 0.0174532925f

namespace Engine {

	using namespace DirectX;

	Camera::Camera(float fov, float screenAspect)
	{
		m_Type = CameraType::Perspective;

		SetPosition(0.0f, 0.0f, -1.1f);
		SetRotation(0.0f, 0.0f, 0.0f);

		m_ScreenAspect = screenAspect;
		m_WorldMatrix = DirectX::XMMatrixIdentity();
		RecalculateViewMatrix();
		m_ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fov, m_ScreenAspect, 0.1f, 10000.0f);
	}

	Camera::Camera(float screenAspect)
	{
		m_Type = CameraType::Ortho;

		SetPosition(0.0f, 0.0f, 0.0f);
		SetRotation(0.0f, 0.0f, 0.0f);

		m_ScreenAspect = screenAspect;
		m_WorldMatrix = DirectX::XMMatrixIdentity();
		RecalculateViewMatrix();
		m_ProjectionMatrix = DirectX::XMMatrixOrthographicLH(screenAspect * 5.0f, 5.0f, 30.f, 10000.0f);
	}

	Camera::~Camera()
	{
	}

	void Camera::SetPosition(float x, float y, float z)
	{
		m_Position.x = x;
		m_Position.y = y;
		m_Position.z = z;

		isChangedView = true;
	}

	void Camera::SetRotation(float x, float y, float z)
	{
		m_Rotation.x = x;
		m_Rotation.y = y;
		m_Rotation.z = z;

		isChangedView = true;
	}

	const DirectX::XMFLOAT3& Camera::GetPosition()
	{
		return m_Position;
	}

	const DirectX::XMFLOAT3& Camera::GetRotation()
	{
		return m_Rotation;
	}

	void Camera::RecalculateViewMatrix()
	{
		if (!isChangedView) return;

		isChangedView = false;
		DirectX::XMFLOAT3 up, position, lookAt;
		DirectX::XMVECTOR upVector, positionVector, lookAtVector;
		float yaw, pitch, roll;
		DirectX::XMMATRIX rotationMatrix;

		up.x = 0.0f;
		up.y = 1.0f;
		up.z = 0.0f;

		upVector = XMLoadFloat3(&up);
		position = m_Position;
		positionVector = DirectX::XMLoadFloat3(&position);

		lookAt.x = 0.0f;
		lookAt.y = 0.0f;
		lookAt.z = 1.0f;
		lookAtVector = XMLoadFloat3(&lookAt);

		pitch = m_Rotation.x * Rad;
		yaw = m_Rotation.y * Rad;
		roll = m_Rotation.z * Rad;

		rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

		lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
		upVector = XMVector3TransformCoord(upVector, rotationMatrix);

		lookAtVector = DirectX::XMVectorAdd(positionVector, lookAtVector);
		m_ViewMatrix = DirectX::XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
	}

	const DirectX::XMMATRIX & Camera::GetViewMatrix()
	{
		RecalculateViewMatrix();
		return m_ViewMatrix;
	}

	const DirectX::XMMATRIX & Camera::GetWorldMatrix()
	{
		return m_WorldMatrix;
	}

	const DirectX::XMMATRIX & Camera::GetProjectionMatrix()
	{
		return m_ProjectionMatrix;
	}

}



