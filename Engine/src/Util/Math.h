#pragma once

namespace Engine::Util {

	std::pair<vec3, vec3> GetTangentAndBinomal(const vec3& inVertex1, const vec3& inVertex2, const vec3& inVertex3,
		const vec2& inUV1, const vec2& inUV2, const vec2& inUV3);

	vec3 GetLookAt(const vec3& rotate);
	vec3 GetRightVector(const vec3& rotate);

	mat4 GetTransform(const vec3& translate, const vec3& rotate, const vec3& scale, bool transpose);
	mat4 GetTransform(const vec3& translate, const vec4& quaternion, const vec3& scale, bool transpose);
	DirectX::XMMATRIX GetTransform(const vec3& translate, const vec3& rotate, const vec3& scale, bool transpose, bool XM);
	DirectX::XMMATRIX GetTransform(const vec3& translate, const vec4& quaternion, const vec3& scale, bool transpose, bool XM);

	vec3 lerp(const vec3 vec1, const vec3 vec2, float ratio);
	DirectX::XMVECTOR lerp(const vec3 vec1, const vec3 vec2, float ratio, bool XM);
	vec4 slerp(const vec4 quat1, const vec4 quat2, float ratio);
	DirectX::XMVECTOR slerp(const vec4 quat1, const vec4 quat2, float ratio, bool XM);

	mat4 GetLerpTransform(const vec3& t1, const vec3& t2, const vec4& q1, const vec4& q2, const vec3& s1, const vec3& s2, float ratio);
	DirectX::XMMATRIX GetLerpTransform(const vec3 & t1, const vec3 & t2, const vec4 & q1, const vec4 & q2, const vec3 & s1, const vec3 & s2, float ratio, bool XM);
	
	mat4 multiply(const mat4& mat1, const mat4& mat2);
	void matrixScaling(mat4& mat, float x, float y, float z);

	mat4 GetOrthographic(float magnification, float screenAspect, float _near, float _far);
	mat4 GetPerspective(float fov, float screenAspect, float _near, float _far);
	mat4 GetViewMatrix(vec3& translate, vec3& rotate);

	void CalcFinalSkinnedTransform(const mat4& offset, mat4& skinnedTransform, float scaleFactor);

	mat4 Translate(const vec3& translate, bool transpose);
	mat4 Rotate(const vec3& rotate, bool transpose);
	mat4 Scale(const vec3& scale, bool transpose);

	void RotateLocalX(vec3& rotate, float radian);
	void RotateLocalY(vec3& rotate, float radian);
	void RotateLocalZ(vec3& rotate, float radian);

	mat4 Transpose(const mat4& mat);
	mat4 Identity();
}