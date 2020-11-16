#include "pch.h"

#include "Transform.h"

namespace Engine {

	using namespace DirectX;

	Transform::Transform()
		: m_RotateValue(0.0f, 0.0f, 0.0f), m_ScaleValue(1.0f, 1.0f, 1.0f), m_TranslateValue(0.0f, 0.0f, 0.0f)
	{
		m_Translate = DirectX::XMMatrixIdentity();
		m_Scale = DirectX::XMMatrixIdentity();
		m_Rotate = DirectX::XMMatrixIdentity();
	}

	Transform::Transform(const DirectX::XMMATRIX & translate, const DirectX::XMMATRIX & rotate, const DirectX::XMMATRIX & scale)
		: m_Translate(translate), m_Rotate(rotate), m_Scale(scale), m_RotateValue(0.0f, 0.0f, 0.0f), m_ScaleValue(0.0f, 0.0f, 0.0f), m_TranslateValue(0.0f, 0.0f, 0.0f)
	{
	}

	void Transform::SetTranslate(float x, float y, float z)
	{
		m_TranslateValue.x = x;
		m_TranslateValue.y = y;
		m_TranslateValue.z = z;
		SetTranslate();
	}

	void Transform::SetRotate(float x, float y, float z)
	{
		m_RotateValue.x = x;
		m_RotateValue.y = y;
		m_RotateValue.z = z;
		SetRotate();
	}

	void Transform::SetScale(float x, float y, float z)
	{
		m_ScaleValue.x = x;
		m_ScaleValue.y = y;
		m_ScaleValue.z = z;
		SetScale();
	}

	void Transform::AddTranslate(float x, float y, float z)
	{
		m_TranslateValue.x += x;
		m_TranslateValue.y += y;
		m_TranslateValue.z += z;
		SetTranslate();
	}

	void Transform::AddRotate(float x, float y, float z)
	{
		m_RotateValue.x += x;
		m_RotateValue.y += y;
		m_RotateValue.z += z;
		SetRotate();
	}

	void Transform::AddScale(float x, float y, float z)
	{
		m_ScaleValue.x += x;
		m_ScaleValue.y += y;
		m_ScaleValue.z += z;
		SetScale();
	}

	void Transform::MoveForwad(float d)
	{
		DirectX::XMVECTOR lookAt = GetFowardVector();

		m_TranslateValue.x += lookAt.m128_f32[0] * d;
		m_TranslateValue.y += lookAt.m128_f32[1] * d;
		m_TranslateValue.z += lookAt.m128_f32[2] * d;

		SetTranslate();
	}

	void Transform::MoveBack(float d)
	{
		DirectX::XMFLOAT3 lookAt;
		DirectX::XMVECTOR lookVector;
		lookAt.x = 0.0f;
		lookAt.y = 0.0f;
		lookAt.z = 1.0f;
		lookVector = XMLoadFloat3(&lookAt);

		lookVector = XMVector3TransformCoord(lookVector, XMMatrixTranspose(m_Rotate));
		DirectX::XMVector3Normalize(lookVector);

		m_TranslateValue.x -= lookVector.m128_f32[0] * d;
		m_TranslateValue.y -= lookVector.m128_f32[1] * d;
		m_TranslateValue.z -= lookVector.m128_f32[2] * d;

		SetTranslate();
	}

	DirectX::XMVECTOR Transform::GetFowardVector() const
	{
		DirectX::XMFLOAT3 lookAt;
		DirectX::XMVECTOR lookVector;
		lookAt.x = 0.0f;
		lookAt.y = 0.0f;
		lookAt.z = 1.0f;
		lookVector = XMLoadFloat3(&lookAt);

		lookVector = XMVector3TransformCoord(lookVector, XMMatrixTranspose(m_Rotate));
		DirectX::XMVector3Normalize(lookVector);

		return lookVector;
	}

	void Transform::SetTranslate()
	{
		m_Translate = DirectX::XMMatrixTranslation(m_TranslateValue.x, m_TranslateValue.y, m_TranslateValue.z);
		m_Translate = DirectX::XMMatrixTranspose(m_Translate);
	}

	void Transform::SetRotate()
	{
		m_Rotate = DirectX::XMMatrixRotationRollPitchYaw(m_RotateValue.x, m_RotateValue.y, m_RotateValue.z);
	}

	void Transform::SetScale()
	{
		m_Scale = DirectX::XMMatrixScaling(m_ScaleValue.x, m_ScaleValue.y, m_ScaleValue.z);
	}

}
