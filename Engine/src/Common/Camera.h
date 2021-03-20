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
		Camera(float fov, float screenAspect, float _near = 0.1f, float _far = 1000.0f, const std::string& name = "Camera");
		//Create Orthographic camera
		Camera(float magnification, float screenAspect, bool dummy, float _near = 0.1f, float _far = 1000.0f, const std::string& name = "Camera");
		~Camera();

		void OnResize(uint32_t width, uint32_t height);
		void SetFov(float fov);
		void SetMagnification(float magnification);
		void SetNear(float _near);
		void SetFar(float _near);
		void SetType(CameraType type);
		void UpdateViewProj();

		Transform& GetTransform();
		const mat4& GetViewMatrix();
		const mat4& GetProjectionMatrix();
		int GetCamType() { return static_cast<int>(m_Type); }
		float GetMag() { return m_Magnification; }
		float GetFov() { return m_Fov; }
		float GetNear() { return m_Near; }
		float GetFar() { return m_Far; }
		float GetScreenAspect() { return m_ScreenAspect; }
		const std::string& GetName() { return m_Cameraname; }
		void SetName(const std::string& name) { m_Cameraname = name; }

		bool isChangedView = false;
		bool isChangedProj = false;

	private:
		void recalculateViewMatrix();
		void recalculateProjectionMatrix();

	private:
		CameraType m_Type = CameraType::None;

		float m_Fov = -1.0f;
		float m_ScreenAspect = -1.0f;
		float m_Magnification = -1.0f; //only for Orthographic
		float m_Near;
		float m_Far;
		
		std::string m_Cameraname;

		Transform m_Transform;
		mat4 m_ViewMatrix;
		mat4 m_ProjectionMatrix;
		friend struct CascadedMatrix;
	};

	
}
