#pragma once
#include "Renderer/CBuffer.h"

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

}
