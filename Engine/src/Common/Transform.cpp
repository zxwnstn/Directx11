#include "pch.h"

#include "Transform.h"
#include "Util/Math.h"

namespace Engine {

	using namespace DirectX;

	Transform::Transform()
	{
		m_Scale.x = 1.0f;
		m_Scale.y = 1.0f;
		m_Scale.z = 1.0f;
	}

	Transform::Transform(const vec3 & translate, const vec3 & rotate, const vec3 & scale)
	{
		m_Translate = translate;
		m_Rotate = rotate;
		m_Scale = scale;
	}

	void Transform::SetTranslate(float x, float y, float z)
	{
		m_Translate.x = x;
		m_Translate.y = y;
		m_Translate.z = z;
	}

	void Transform::SetTranslate(const vec3 & translate)
	{
		m_Translate = translate;
	}

	void Transform::SetRotate(float x, float y, float z)
	{
		m_Rotate.x = x;
		m_Rotate.y = y;
		m_Rotate.z = z;
	}

	void Transform::SetRotate(const vec3 & rotate)
	{
		m_Rotate = rotate;
	}

	void Transform::SetScale(float x, float y, float z)
	{
		m_Scale.x = x;
		m_Scale.y = y;
		m_Scale.z = z;
	}

	void Transform::SetScale(const vec3 & scale)
	{
		m_Scale = scale;
	}

	void Transform::AddTranslate(float x, float y, float z)
	{
		m_Translate.x += x;
		m_Translate.y += y;
		m_Translate.z += z;
	}

	void Transform::AddTranslate(const vec3 & position)
	{
		m_Translate.x += position.x;
		m_Translate.y += position.y;
		m_Translate.z += position.z;
	}

	void Transform::AddRotate(float x, float y, float z)
	{
		m_Rotate.x += x;
		m_Rotate.y += y;
		m_Rotate.z += z;
	}

	void Transform::AddRotate(const vec3 & rotation)
	{
		m_Rotate.x += rotation.x;
		m_Rotate.y += rotation.y;
		m_Rotate.z += rotation.z;
	}

	void Transform::AddScale(float x, float y, float z)
	{
		m_Scale.x += x;
		m_Scale.y += y;
		m_Scale.z += z;
	}

	void Transform::AddScale(const vec3 & scale)
	{
		m_Scale.x += scale.x;
		m_Scale.y += scale.y;
		m_Scale.z += scale.z;
	}

	void Transform::LocalRotateX(float radian)
	{
		Util::RotateLocalX(m_Rotate, radian);
	}

	void Transform::LocalRotateY(float radian)
	{
		Util::RotateLocalY(m_Rotate, radian);
	}

	void Transform::LocalRotateZ(float radian)
	{
		Util::RotateLocalZ(m_Rotate, radian);
	}

	void Transform::MoveForwad(float d)
	{
		vec3 lookAt = GetLookAtVector();

		m_Translate.x += lookAt.x * d;
		m_Translate.y += lookAt.y * d;
		m_Translate.z += lookAt.z * d;
	}

	void Transform::MoveBack(float d)
	{
		vec3 lookAt = GetLookAtVector();

		m_Translate.x -= lookAt.x * d;
		m_Translate.y -= lookAt.y * d;
		m_Translate.z -= lookAt.z * d;
	}

	void Transform::MoveLeft(float d)
	{
		vec3 right = Util::GetRightVector(m_Rotate);

		m_Translate.x -= right.x * d;
		m_Translate.y -= right.y * d;
		m_Translate.z -= right.z * d;
	}

	void Transform::MoveRight(float d)
	{
		vec3 right = Util::GetRightVector(m_Rotate);

		m_Translate.x += right.x * d;
		m_Translate.y += right.y * d;
		m_Translate.z += right.z * d;
	}

	vec3 Transform::GetLookAtVector() const
	{
		return Util::GetLookAt(m_Rotate);
	}


}
