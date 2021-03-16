#include "pch.h"

#include "Light.h"
#include "Util/Math.h"

namespace Engine {

	Engine::Environment::Environment()
	{
		WorldMatrix = Util::Identity();
	}

	Light::Light()
		: lightCam(3.141592f / 2.0f, 1.0f)
	{
	}

	void Light::UpdateCascade(std::shared_ptr<Camera> curcamera)
	{
		m_CascadedMat.Update(lightCam.GetTransform().GetRotate(), curcamera);
	}


	CascadedMatrix::CascadedMatrix()
	{
		m_iShadowMapSize = 4096;

		// Set the range values
		m_arrCascadeRanges[0] = 0.1f;
		m_arrCascadeRanges[1] = 2.0f;
		m_arrCascadeRanges[2] = 10.0f;
		m_arrCascadeRanges[3] = 1000.0f;
	}


	void CascadedMatrix::Update(const vec3 & directionalDir, std::shared_ptr<Camera> curcam)
	{
		m_arrCascadeRanges[0] = curcam->m_Near;
		m_arrCascadeRanges[3] = curcam->m_Far;

		mat4 viewInverse = Util::Inverse(curcam->m_ViewMatrix);
		vec3 vDirectionalDir = directionalDir;
		// Find the view matrix

		auto vwc = viewInverse._4 + viewInverse._3 * m_fCascadeTotalRange * 0.5f;
		vec3 vWorldCenter{ vwc.x, vwc.y, vwc.z };
		vec3 vPos = vWorldCenter;
		mat4 mShadowView = Util::GetViewMatrix(vPos, vDirectionalDir);

		// Get the bounds for the shadow space
		float fRadius;
		ExtractFrustumBoundSphere(m_arrCascadeRanges[0], m_arrCascadeRanges[3], m_vShadowBoundCenter, fRadius, viewInverse, curcam);
		m_fShadowBoundRadius = max(m_fShadowBoundRadius, fRadius); // Expend the radius to compensate for numerical errors

		// Find the projection matrix
		mat4 mShadowProj = Util::GetOrthographic(m_fShadowBoundRadius, 1.0f, -m_fShadowBoundRadius, m_fShadowBoundRadius);

		// The combined transformation from world to shadow space
		m_WorldToShadowSpace = Util::multiply(mShadowView, mShadowProj);

		// For each cascade find the transformation from shadow to cascade space
		mat4 mShadowViewInv = Util::Inverse(mShadowView);
		for (int iCascadeIdx = 0; iCascadeIdx < m_iTotalCascades; iCascadeIdx++)
		{
			mat4 cascadeTrans;
			mat4 cascadeScale;

			// Extract the bounding box
			vec3 arrFrustumPoints[8];
			ExtractFrustumPoints(m_arrCascadeRanges[iCascadeIdx], m_arrCascadeRanges[iCascadeIdx + 1], arrFrustumPoints, viewInverse, curcam);

			// Transform to shadow space and extract the minimum andn maximum
			vec3 vMin{ FLT_MAX, FLT_MAX, FLT_MAX };
			vec3 vMax{ -FLT_MAX, -FLT_MAX, -FLT_MAX };
			for (int i = 0; i < 8; i++)
			{
				vec3 vPointInShadowSpace = Util::TransformCoord(m_WorldToShadowSpace, arrFrustumPoints[i]);
				for (int j = 0; j < 3; j++)
				{
					if (vMin.m[j] > vPointInShadowSpace.m[j])
						vMin.m[j] = vPointInShadowSpace.m[j];
					if (vMax.m[j] < vPointInShadowSpace.m[j])
						vMax.m[j] = vPointInShadowSpace.m[j];
				}
			}

			vec3 vCascadeCenterShadowSpace = (vMin + vMax) * 0.5f;

			// Update the translation from shadow to cascade space
			m_vToCascadeOffsetX.m[iCascadeIdx] = -vCascadeCenterShadowSpace.x;
			m_vToCascadeOffsetY.m[iCascadeIdx] = -vCascadeCenterShadowSpace.y;
			cascadeTrans = Util::Translate({ -vCascadeCenterShadowSpace.x, -vCascadeCenterShadowSpace.y, 0.0f }, false);

			// Update the scale from shadow to cascade space
			m_vToCascadeScale.m[iCascadeIdx] = 2.0f / max(vMax.x - vMin.x, vMax.y - vMin.y);
			cascadeScale = Util::Scale({ m_vToCascadeScale.m[iCascadeIdx], m_vToCascadeScale.m[iCascadeIdx], 1.0f }, false);

			// Combine the matrices to get the transformation from world to cascade space
			m_arrWorldToCascadeProj[iCascadeIdx] = Util::multiply(m_WorldToShadowSpace, Util::multiply(cascadeTrans, cascadeScale));
		}

		// Set the values for the unused slots to someplace outside the shadow space
		for (int i = m_iTotalCascades; i < 4; i++)
		{
			m_vToCascadeOffsetX.m[i] = 500.0f;
			m_vToCascadeOffsetY.m[i] = 500.0f;
			m_vToCascadeScale.m[i] = 0.1f;
		}
	}

	void CascadedMatrix::ExtractFrustumPoints(float fNear, float fFar, vec3* arrFrustumCorners, mat4& viewInverse, std::shared_ptr<Camera> curcam)
	{
		// Get the camera bases
		vec3 camPos{ viewInverse._4.x, viewInverse._4.y, viewInverse._4.z };
		vec3 camRight{ viewInverse._1.x, viewInverse._1.y, viewInverse._1.z };
		vec3 camUp{ viewInverse._2.x, viewInverse._2.y, viewInverse._2.z };
		vec3 camForward{ viewInverse._3.x, viewInverse._3.y, viewInverse._3.z };

		// Calculate the tangent values (this can be cached
		const float fTanFOVX = tanf(curcam->m_ScreenAspect * curcam->m_Fov);
		const float fTanFOVY = tanf(curcam->m_ScreenAspect);

		// Calculate the points on the near plane
		arrFrustumCorners[0] = camPos + (-camRight * fTanFOVX + camUp * fTanFOVY + camForward) * fNear;
		arrFrustumCorners[1] = camPos + (camRight * fTanFOVX + camUp * fTanFOVY + camForward) * fNear;
		arrFrustumCorners[2] = camPos + (camRight * fTanFOVX - camUp * fTanFOVY + camForward) * fNear;
		arrFrustumCorners[3] = camPos + (-camRight * fTanFOVX - camUp * fTanFOVY + camForward) * fNear;

		// Calculate the points on the far plane
		arrFrustumCorners[4] = camPos + (-camRight * fTanFOVX + camUp * fTanFOVY + camForward) * fFar;
		arrFrustumCorners[5] = camPos + (camRight * fTanFOVX + camUp * fTanFOVY + camForward) * fFar;
		arrFrustumCorners[6] = camPos + (camRight * fTanFOVX - camUp * fTanFOVY + camForward) * fFar;
		arrFrustumCorners[7] = camPos + (-camRight * fTanFOVX - camUp * fTanFOVY + camForward) * fFar;
	}

	void CascadedMatrix::ExtractFrustumBoundSphere(float fNear, float fFar, vec3 & vBoundCenter, float & fBoundRadius, mat4& viewInverse, std::shared_ptr<Camera> curcam)
	{
		// Get the camera bases
		vec3 camPos{ viewInverse._4.x, viewInverse._4.y, viewInverse._4.z };
		vec3 camRight{ viewInverse._1.x, viewInverse._1.y, viewInverse._1.z };
		vec3 camUp{ viewInverse._2.x, viewInverse._2.y, viewInverse._2.z };
		vec3 camForward{ viewInverse._3.x, viewInverse._3.y, viewInverse._3.z };

		// Calculate the tangent values (this can be cached as long as the FOV doesn't change)
		const float fTanFOVX = tanf(curcam->m_ScreenAspect * curcam->m_Fov);
		const float fTanFOVY = tanf(curcam->m_ScreenAspect);

		// The center of the sphere is in the center of the frustum
		vBoundCenter = camPos + camForward * (fNear + 0.5f * (fNear + fFar));

		// Radius is the distance to one of the frustum far corners
		const vec3 vBoundSpan = camPos + (-camRight * fTanFOVX + camUp * fTanFOVY + camForward) * fFar - vBoundCenter;
		fBoundRadius = Util::Length(vBoundSpan);
	}

}
