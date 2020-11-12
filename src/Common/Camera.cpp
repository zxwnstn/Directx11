#include "pch.h"

#include "Camera.h"

#define Rad 0.0174532925f

using namespace DirectX;

//Camera::Camera()
//{
//	SetProj(0.25f*3.14f, 1.0f, 0.01f, 1000.0f);
//	UpdateViewMatrix();
//}
//
//Camera::~Camera()
//{
//}
//
//void Camera::SetProj(float fovY, float aspect, float zn, float zf)
//{
//	// cache properties
//	mFovY = fovY;
//	mAspect = aspect;
//	mNearZ = zn;
//	mFarZ = zf;
//
//	mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f*mFovY);
//	mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f*mFovY);
//
//	XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
//	XMStoreFloat4x4(&mProj, P);
//}
//
//void Camera::AddYaw(float dx)
//{
//	XMMATRIX R = XMMatrixRotationY(dx);
//	XMVECTOR EyeRight = DirectX::XMVector3TransformNormal(XMLoadFloat3(&mEyeRight), R);
//	XMVECTOR EyeUp = DirectX::XMVector3TransformNormal(XMLoadFloat3(&mEyeUp), R);
//	XMVECTOR EyeLook = DirectX::XMVector3TransformNormal(XMLoadFloat3(&mEyeLook), R);
//
//	XMStoreFloat3(&mEyeRight, EyeRight);
//	XMStoreFloat3(&mEyeUp, EyeUp);
//	XMStoreFloat3(&mEyeLook, EyeLook);
//
//	mViewDirty = true;
//}
//void Camera::AddPitch(float dy)
//{
//	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&mEyeRight), dy);
//
//	XMStoreFloat3(&mEyeUp, XMVector3TransformNormal(XMLoadFloat3(&mEyeUp), R));
//	XMStoreFloat3(&mEyeLook, XMVector3TransformNormal(XMLoadFloat3(&mEyeLook), R));
//
//	mViewDirty = true;
//}
//void Camera::Walk(float velocity)
//{
//	XMVECTOR Velocity = XMVectorSet(velocity, velocity, velocity, velocity);
//	XMVECTOR Position = XMLoadFloat3(&mEyePosition);
//	XMVECTOR Look = XMLoadFloat3(&mEyeLook);
//	XMStoreFloat3(&mEyePosition, XMVectorMultiplyAdd(Velocity, Look, Position));
//
//	mViewDirty = true;
//}
//void Camera::WalkSideway(float inVelocity)
//{
//	XMVECTOR Velocity = XMVectorSet(inVelocity, inVelocity, inVelocity, inVelocity);
//	XMVECTOR Position = XMLoadFloat3(&mEyePosition);
//	XMVECTOR Right = XMLoadFloat3(&mEyeRight);
//	XMStoreFloat3(&mEyePosition, XMVectorMultiplyAdd(Velocity, Right, Position));
//
//	mViewDirty = true;
//}
//
//XMVECTOR Camera::GetEyePosition() const
//{
//	return XMLoadFloat3(&mEyePosition);
//}
//XMFLOAT3 Camera::GetEyePosition3f() const
//{
//	return mEyePosition;
//}
//XMVECTOR Camera::GetEyeLook() const
//{
//	return XMLoadFloat3(&mEyeLook);
//}
//XMVECTOR Camera::GetEyeUp() const
//{
//	return XMLoadFloat3(&mEyeUp);
//}
//XMVECTOR Camera::GetEyeRight() const
//{
//	return XMLoadFloat3(&mEyeRight);
//}
//XMMATRIX Camera::GetView() const
//{
//	return XMLoadFloat4x4(&mView);
//}
//XMMATRIX Camera::GetProj() const
//{
//	return XMLoadFloat4x4(&mProj);
//}
//
//void Camera::SetEyePosition(DirectX::XMVECTOR inEyePosition)
//{
//	XMStoreFloat3(&mEyePosition, inEyePosition);
//}
//void Camera::SetEyeLook(DirectX::XMVECTOR inEyeLook)
//{
//	XMStoreFloat3(&mEyeLook, inEyeLook);
//}
//void Camera::SetEyeUp(DirectX::XMVECTOR inEyeUp)
//{
//	XMStoreFloat3(&mEyeUp, inEyeUp);
//}
//void Camera::SetEyeRight(DirectX::XMVECTOR inEyeRight)
//{
//	XMStoreFloat3(&mEyeRight, inEyeRight);
//}
//
//void Camera::UpdateViewMatrix()
//{
//	if (mViewDirty)
//	{
//		XMVECTOR P = XMLoadFloat3(&mEyePosition);
//		XMVECTOR L = XMLoadFloat3(&mEyeLook);
//		XMVECTOR U = XMLoadFloat3(&mEyeUp);
//		XMVECTOR R = XMLoadFloat3(&mEyeRight);
//
//		// Keep camera's axes orthogonal to each other and of unit length.
//		L = XMVector3Normalize(L);
//		U = XMVector3Normalize(XMVector3Cross(L, R));
//
//		// U, L already ortho-normal, so no need to normalize cross product.
//		R = XMVector3Cross(U, L);
//
//		// Fill in the view matrix entries.
//		float x = -XMVectorGetX(XMVector3Dot(P, R));
//		float y = -XMVectorGetX(XMVector3Dot(P, U));
//		float z = -XMVectorGetX(XMVector3Dot(P, L));
//
//		XMStoreFloat3(&mEyeRight, R);
//		XMStoreFloat3(&mEyeUp, U);
//		XMStoreFloat3(&mEyeLook, L);
//
//		mView(0, 0) = mEyeRight.x;
//		mView(1, 0) = mEyeRight.y;
//		mView(2, 0) = mEyeRight.z;
//		mView(3, 0) = x;
//
//		mView(0, 1) = mEyeUp.x;
//		mView(1, 1) = mEyeUp.y;
//		mView(2, 1) = mEyeUp.z;
//		mView(3, 1) = y;
//
//		mView(0, 2) = mEyeLook.x;
//		mView(1, 2) = mEyeLook.y;
//		mView(2, 2) = mEyeLook.z;
//		mView(3, 2) = z;
//
//		mView(0, 3) = 0.0f;
//		mView(1, 3) = 0.0f;
//		mView(2, 3) = 0.0f;
//		mView(3, 3) = 1.0f;
//
//		mViewDirty = false;
//	}
//}

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

