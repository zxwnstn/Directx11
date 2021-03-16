#pragma once

#include "Camera.h"

namespace Engine {

	struct Environment
	{
		vec3 Ambient{ 0.0f, 0.0f, 0.0f};
		mat4 WorldMatrix;
		bool UseShadowMap = false;
		vec4 bias;
		Environment();
	};

	struct CascadedMatrix
	{
	public:
		CascadedMatrix();
		void Update(const vec3& directionalDir, std::shared_ptr<Camera> curcamera);

	private:
		void ExtractFrustumPoints(float fNear, float fFar, vec3* arrFrustumCorners, mat4& viewInvers, std::shared_ptr<Camera> curcam);
		void ExtractFrustumBoundSphere(float fNear, float fFar, vec3& vBoundCenter, float& fBoundRadius, mat4& viewInverse, std::shared_ptr<Camera> curcam);

	public:
		static const int m_iTotalCascades = 3;

		bool m_bAntiFlickerOn;
		int m_iShadowMapSize;
		float m_fCascadeTotalRange = 50.0f;
		float m_arrCascadeRanges[4];

		vec3 m_vShadowBoundCenter;
		float m_fShadowBoundRadius;
		vec3 m_arrCascadeBoundCenter[3];
		float m_arrCascadeBoundRadius[3];

		mat4 m_WorldToShadowSpace;
		mat4 m_arrWorldToCascadeProj[3];

		vec4 m_vToCascadeOffsetX;
		vec4 m_vToCascadeOffsetY;
		vec4 m_vToCascadeScale;
	};

	struct Light
	{
		Light();
		enum Type : unsigned int
		{
			Directional,
			Point,
			Spot
		};
		void UpdateCascade(std::shared_ptr<Camera> curcamera);

		CascadedMatrix m_CascadedMat;
		Camera lightCam;
		std::string name;
		vec4 m_Color{1.0f, 1.0f, 1.0f, 1.0f};
		Type m_Type = Type::Directional;
		float m_InnerAngle = 0.0f;
		float m_OuterAngle = 1.0f;
		float m_Intensity = 1;
		float m_Range = 10.0f;

		bool noLight = false;
	};

}
