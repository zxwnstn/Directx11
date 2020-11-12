#pragma once
#include "Renderer/CBuffer.h"

enum class CameraType
{
	Ortho,
	Perspective,
	None
};

//class Camera
//{
//public:
//	Camera();
//	~Camera();
//
//	void SetProj(float fovY, float aspect, float zn, float zf);
//
//	void AddYaw(float dx);
//	void AddPitch(float dy);
//
//	void Walk(float velocity);
//	void WalkSideway(float velocity);
//
//	DirectX::XMVECTOR GetEyePosition() const;
//	DirectX::XMFLOAT3 GetEyePosition3f() const;
//	DirectX::XMVECTOR GetEyeLook() const;
//	DirectX::XMVECTOR GetEyeUp() const;
//	DirectX::XMVECTOR GetEyeRight() const;
//	DirectX::XMMATRIX GetView() const;
//	DirectX::XMMATRIX GetProj() const;
//
//	void SetEyePosition(DirectX::XMVECTOR inEyePosition);
//	void SetEyeLook(DirectX::XMVECTOR inEyeLook);
//	void SetEyeUp(DirectX::XMVECTOR inEyeUp);
//	void SetEyeRight(DirectX::XMVECTOR inEyeRight);
//
//	void UpdateViewMatrix();
//
//	bool mViewDirty = true;
//
//private:
//	DirectX::XMFLOAT4X4 mView;
//	DirectX::XMFLOAT4X4 mProj;
//
//	/*
//	* EyePos - EyeTarget
//	* a d - playerTarget Move(Yaw)
//	* w s - playerTarget, EyePos Move // EyeDirection based on playerTarget
//	* mouse click - EyeTarget Move
//	*/
//	 Character View
//	DirectX::XMFLOAT3 mEyePosition = { 0.0f, 0.0f, 0.0f };
//	DirectX::XMFLOAT3 mEyeLook = { 0.0f, 0.0f, 1.0f };
//	DirectX::XMFLOAT3 mEyeUp = { 0.0f, 1.0f, 0.0f };
//	DirectX::XMFLOAT3 mEyeRight = { 1.0f, 0.0f, 0.0f };
//
//	 Cache frustum properties.
//	float mNearZ = 0.0f;
//	float mFarZ = 0.0f;
//	float mAspect = 0.0f;
//	float mFovY = 0.0f;
//	float mNearWindowHeight = 0.0f;
//	float mFarWindowHeight = 0.0f;
//
//	friend class SandBox;
//};

class Camera
{
public:
	Camera(float fov, float screenAspect);
	Camera(float screenAspect);
	~Camera();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	const DirectX::XMFLOAT3& GetPosition();
	const DirectX::XMFLOAT3& GetRotation();

	void RecalculateViewMatrix();
	const DirectX::XMMATRIX& GetViewMatrix();
	const DirectX::XMMATRIX& GetWorldMatrix();
	const DirectX::XMMATRIX& GetProjectionMatrix();

	inline operator CBuffer::Camera()
	{
		RecalculateViewMatrix();

		DirectX::XMFLOAT4 position{ m_Position.x, m_Position.y, m_Position.z, 1.0f };
		CBuffer::Camera camera{ DirectX::XMMatrixTranspose(m_WorldMatrix), DirectX::XMMatrixTranspose(m_ViewMatrix), DirectX::XMMatrixTranspose(m_ProjectionMatrix), position };
		return camera;
	}

private:
	bool isChangedView = false;

	CameraType m_Type = CameraType::None;

	float m_ScreenAspect;
	DirectX::XMFLOAT3 m_Rotation;
	DirectX::XMFLOAT3 m_Position;
	DirectX::XMMATRIX m_WorldMatrix;
	DirectX::XMMATRIX m_ViewMatrix;
	DirectX::XMMATRIX m_ProjectionMatrix;
};