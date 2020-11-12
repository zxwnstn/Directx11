#include "pch.h"

#include "Transform.h"



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


