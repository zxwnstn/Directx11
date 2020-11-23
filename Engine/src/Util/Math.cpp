#include "pch.h"

#include "Math.h"
#include "MathTypeConverter.h"

namespace Engine::Util {

	DirectX::XMVECTOR zero{ 0.0f, 0.0f, 0.0f, 1.0f };

	std::pair<vec3, vec3> GetTangentAndBinomal(const vec3 & inVertex1, const vec3 & inVertex2, const vec3 & inVertex3, const vec2 & inUV1, const vec2 & inUV2, const vec2 & inUV3)
	{
		vec3 vector1, vector2;
		for (int i = 0; i < 3; ++i)
		{
			vector1.m[i] = inVertex2.m[i] - inVertex1.m[i];
			vector2.m[i] = inVertex3.m[i] - inVertex1.m[i];
		}

		vec2 tuVector, tvVector;
		// Calculate the tu and tv texture space vectors.
		tuVector.m[0] = inUV2.m[0] - inUV1.m[0];
		tvVector.m[0] = inUV2.m[1] - inUV1.m[1];

		tuVector.m[1] = inUV3.m[0] - inUV1.m[0];
		tvVector.m[1] = inUV3.m[1] - inUV1.m[1];

		// Calculate the denominator of the tangent/binormal equation.
		float den = 1.0f / (tuVector.m[0] * tvVector.m[1] - tuVector.m[1] * tvVector.m[0]);

		#ifdef DEBUD_FEATURE
		if (isnan(den) || isinf(den))
		{
			LOG_WARN("FBXLoader::Tangent and Binormal are calculated invalid")
				return false;
		}
		#endif	

		vec3 outTangent, outBinormal;

		// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
		outTangent.m[0] = (tvVector.m[1] * vector1.m[0] - tvVector.m[0] * vector2.m[0]) * den;
		outTangent.m[1] = (tvVector.m[1] * vector1.m[1] - tvVector.m[0] * vector2.m[1]) * den;
		outTangent.m[2] = (tvVector.m[1] * vector1.m[2] - tvVector.m[0] * vector2.m[2]) * den;
		outBinormal.m[0] = (tuVector.m[0] * vector2.m[0] - tuVector.m[1] * vector1.m[0]) * den;
		outBinormal.m[1] = (tuVector.m[0] * vector2.m[1] - tuVector.m[1] * vector1.m[1]) * den;
		outBinormal.m[2] = (tuVector.m[0] * vector2.m[2] - tuVector.m[1] * vector1.m[2]) * den;

		//Normalize
		float length = sqrt((outTangent.m[0] * outTangent.m[0]) + (outTangent.m[1] * outTangent.m[1]) + (outTangent.m[2] * outTangent.m[2]));
		outTangent.m[0] = outTangent.m[0] / length;
		outTangent.m[1] = outTangent.m[1] / length;
		outTangent.m[2] = outTangent.m[2] / length;

		length = sqrt((outBinormal.m[0] * outBinormal.m[0]) + (outBinormal.m[1] * outBinormal.m[1]) + (outBinormal.m[2] * outBinormal.m[2]));
		outBinormal.m[0] = outBinormal.m[0] / length;
		outBinormal.m[1] = outBinormal.m[1] / length;
		outBinormal.m[2] = outBinormal.m[2] / length;

		#ifdef DEBUD_FEATURE
		for (int i = 0; i < 3; ++i)
		{
			ASSERT(!(isnan(binormal.m[i]) || isnan(tangent.m[i])), "FBXLoader::Binormal or Tangent value is invalid!");
			return false;
		}
		#endif

		return { outTangent, outBinormal};
	}

	vec3 GetLookAt(const vec3 & rotate)
	{
		DirectX::XMVECTOR lookVector;
		DirectX::XMMATRIX rotateMat;

		lookVector.m128_f32[0] = 0.0f;
		lookVector.m128_f32[1] = 0.0f;
		lookVector.m128_f32[2] = 1.0f;
		lookVector.m128_f32[3] = 0.0f;

		rotateMat = DirectX::XMMatrixRotationRollPitchYaw(rotate.x, rotate.y, rotate.z);
		lookVector = DirectX::XMVector3TransformCoord(lookVector, DirectX::XMMatrixTranspose(rotateMat));
		DirectX::XMVector3Normalize(lookVector);

		return ToVector3(lookVector);
	}
	
	mat4 GetTransform(const vec3 & translate, const vec3 & rotate, const vec3 & scale, bool transpose)
	{
		DirectX::XMVECTOR t = ToXMVector(translate);
		DirectX::XMVECTOR q = ToQuaternion(rotate);
		DirectX::XMVECTOR s = ToXMVector(scale);

		DirectX::XMMATRIX ret = DirectX::XMMatrixAffineTransformation(s, zero, q, t);

		if (transpose)
			ret = DirectX::XMMatrixTranspose(ret);

		return ToMatrix(ret);
	}

	mat4 GetTransform(const vec3 & translate, const vec4 & quaternion, const vec3 & scale, bool transpose)
	{
		DirectX::XMVECTOR t = ToXMVector(translate);
		DirectX::XMVECTOR q = ToXMVector(quaternion);
		DirectX::XMVECTOR s = ToXMVector(scale);

		DirectX::XMMATRIX ret = DirectX::XMMatrixAffineTransformation(s, zero, q, t);

		if (transpose)
			ret = DirectX::XMMatrixTranspose(ret);

		return ToMatrix(ret);
	}

	DirectX::XMMATRIX GetTransform(const vec3 & translate, const vec3 & rotate, const vec3 & scale, bool transpose, bool XM)
	{
		DirectX::XMVECTOR t = ToXMVector(translate);
		DirectX::XMVECTOR q = ToQuaternion(rotate);
		DirectX::XMVECTOR s = ToXMVector(scale);

		DirectX::XMMATRIX ret = DirectX::XMMatrixAffineTransformation(s, zero, q, t);

		if (transpose)
			ret = DirectX::XMMatrixTranspose(ret);

		return ret;
	}
	DirectX::XMMATRIX GetTransform(const vec3 & translate, const vec4 & quaternion, const vec3 & scale, bool transpose, bool XM)
	{
		DirectX::XMVECTOR t = ToXMVector(translate);
		DirectX::XMVECTOR q = ToXMVector(quaternion);
		DirectX::XMVECTOR s = ToXMVector(scale);

		DirectX::XMMATRIX ret = DirectX::XMMatrixAffineTransformation(s, zero, q, t);

		if (transpose)
			ret = DirectX::XMMatrixTranspose(ret);

		return ret;
	}
	vec3 lerp(const vec3 vec1, const vec3 vec2, float ratio)
	{
		DirectX::XMVECTOR ret;
		DirectX::XMVECTOR v1 = ToXMVector(vec1);
		DirectX::XMVECTOR v2 = ToXMVector(vec2);
		ret = DirectX::XMVectorLerp(v1, v2, ratio);

		return ToVector3(ret);
	}
	DirectX::XMVECTOR lerp(const vec3 vec1, const vec3 vec2, float ratio, bool XM)
	{
		DirectX::XMVECTOR ret;
		DirectX::XMVECTOR v1 = ToXMVector(vec1);
		DirectX::XMVECTOR v2 = ToXMVector(vec2);
		ret = DirectX::XMVectorLerp(v1, v2, ratio);

		return ret;
	}
	vec4 slerp(const vec4 quat1, const vec4 quat2, float ratio)
	{
		DirectX::XMVECTOR ret;
		DirectX::XMVECTOR v1 = ToXMVector(quat1);
		DirectX::XMVECTOR v2 = ToXMVector(quat2);
		ret = DirectX::XMQuaternionSlerp(v1, v2, ratio);

		return ToVector4(ret);
	}
	DirectX::XMVECTOR slerp(const vec4 quat1, const vec4 quat2, float ratio, bool XM)
	{
		DirectX::XMVECTOR ret;
		DirectX::XMVECTOR v1 = ToXMVector(quat1);
		DirectX::XMVECTOR v2 = ToXMVector(quat2);
		ret = DirectX::XMQuaternionSlerp(v1, v2, ratio);

		return ret;
	}
	mat4 GetLerpTransform(const vec3 & t1, const vec3 & t2, const vec4 & q1, const vec4 & q2, const vec3 & s1, const vec3 & s2, float ratio)
	{
		DirectX::XMVECTOR t = lerp(t1, t2, ratio, true);
		DirectX::XMVECTOR q = slerp(q1, q2, ratio, true);
		DirectX::XMVECTOR s = lerp(s1, s2, ratio, true);
		DirectX::XMMATRIX ret = DirectX::XMMatrixAffineTransformation(s, zero, q, t);

		return ToMatrix(ret);
	}
	DirectX::XMMATRIX GetLerpTransform(const vec3 & t1, const vec3 & t2, const vec4 & q1, const vec4 & q2, const vec3 & s1, const vec3 & s2, float ratio, bool XM)
	{
		DirectX::XMVECTOR t = lerp(t1, t2, ratio, true);
		DirectX::XMVECTOR q = slerp(q1, q2, ratio, true);
		DirectX::XMVECTOR s = lerp(s1, s2, ratio, true);
		DirectX::XMMATRIX ret = DirectX::XMMatrixAffineTransformation(s, zero, q, t);

		return ret;
	}

	mat4 multiply(const mat4 & mat1, const mat4 & mat2)
	{
		DirectX::XMMATRIX m1 = ToXMMatrix(mat1);
		DirectX::XMMATRIX m2 = ToXMMatrix(mat2);
		DirectX::XMMATRIX ret = DirectX::XMMatrixMultiply(m1, m2);

		return ToMatrix(ret);
	}

	void matrixScaling(mat4 & mat, float x, float y, float z)
	{
		DirectX::XMMATRIX m = ToXMMatrix(mat);
		DirectX::XMMATRIX s = DirectX::XMMatrixScaling(x, y, z);
		DirectX::XMMATRIX ret = DirectX::XMMatrixMultiply(m, s);

		mat = ToMatrix(ret);
	}

	mat4 GetOrthographic(float magnification, float screenAspect, float _near, float _far)
	{
		DirectX::XMMATRIX ret = DirectX::XMMatrixOrthographicLH(screenAspect * magnification, magnification, _near, _far);
		return ToMatrix(ret);
	}

	mat4 GetPerspective(float fov, float screenAspect, float _near, float _far)
	{
		DirectX::XMMATRIX ret = DirectX::XMMatrixPerspectiveFovLH(fov, screenAspect, _near, _far);
		return ToMatrix(ret);
	}

	mat4 GetViewMatrix(vec3 & translate, vec3 & rotate)
	{
		DirectX::XMMATRIX ret;
		DirectX::XMVECTOR upVector, positionVector, lookAtVector;
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotate.x, rotate.y, rotate.z);

		//position vector
		positionVector = ToXMVector(translate);
		
		//look at vector
		lookAtVector.m128_f32[0] = 0.0f;
		lookAtVector.m128_f32[1] = 0.0f;
		lookAtVector.m128_f32[2] = 1.0f;
		lookAtVector.m128_f32[3] = 0.0f;
		lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
		lookAtVector = DirectX::XMVectorAdd(positionVector, lookAtVector);

		//up vector
		upVector.m128_f32[0] = 0.0f;
		upVector.m128_f32[1] = 1.0f;
		upVector.m128_f32[2] = 0.0f;
		upVector.m128_f32[3] = 0.0f;
		upVector = XMVector3TransformCoord(upVector, rotationMatrix);

		ret = DirectX::XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
		return ToMatrix(ret);

	}

	void CalcFinalSkinnedTransform(const mat4 & offset, mat4 & skinnedTransform, float scaleFactor)
	{
		DirectX::XMMATRIX offsetMat = ToXMMatrix(offset);
		DirectX::XMMATRIX skinnedMat = ToXMMatrix(skinnedTransform);
		DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);

		DirectX::XMMATRIX ret = offsetMat * skinnedMat * scaleMat;
		ret = DirectX::XMMatrixTranspose(ret);
		skinnedTransform = ToMatrix(ret);
	}

	mat4 Translate(const vec3 & translate, bool transpose)
	{
		DirectX::XMMATRIX ret;
		ret = DirectX::XMMatrixTranslation(translate.x, translate.y, translate.z);

		if (transpose)
			ret = DirectX::XMMatrixTranspose(ret);

		return ToMatrix(ret);
	}

	mat4 Rotate(const vec3 & rotate, bool transpose)
	{
		DirectX::XMMATRIX ret;
		ret = DirectX::XMMatrixRotationRollPitchYaw(rotate.x, rotate.y, rotate.z);

		if (transpose)
			ret = DirectX::XMMatrixTranspose(ret);

		return ToMatrix(ret);
	}

	mat4 Scale(const vec3 & scale, bool transpose)
	{
		DirectX::XMMATRIX ret;
		ret = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

		if (transpose)
			ret = DirectX::XMMatrixTranspose(ret);

		return ToMatrix(ret);
	}

	mat4 Transpose(const mat4& mat)
	{
		DirectX::XMMATRIX ret = ToXMMatrix(mat);
		ret = DirectX::XMMatrixTranspose(ret);
		return ToMatrix(ret);
	}

	mat4 Identity()
	{
		mat4 ret;
		ret.m[0][0] = 1.0f;
		ret.m[1][1] = 1.0f;
		ret.m[2][2] = 1.0f;
		ret.m[3][3] = 1.0f;
		return ret;
	}
}