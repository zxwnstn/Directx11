#pragma once

namespace Engine {

	class Transform
	{
	public:
		Transform();
		Transform(const DirectX::XMMATRIX& translate, const DirectX::XMMATRIX& rotate, const DirectX::XMMATRIX& scale);

		void SetTranslate(float x, float y, float z);
		void SetRotate(float x, float y, float z);
		void SetScale(float x, float y, float z);

		void AddTranslate(float x, float y, float z);
		void AddRotate(float x, float y, float z);
		void AddScale(float x, float y, float z);

		void MoveForwad(float d);
		void MoveBack(float d);

		inline const DirectX::XMMATRIX& GetTranslate() const { return m_Translate; }
		inline const DirectX::XMMATRIX& GetRotate() const { return m_Rotate; }
		inline const DirectX::XMMATRIX& GetScale() const { return m_Scale; }

		inline const DirectX::XMFLOAT3 GetTranslateValue() const { return m_TranslateValue; }
		inline const DirectX::XMFLOAT3 GetRotationValue() const { return m_RotateValue; }
		inline const DirectX::XMFLOAT3 GetScaleValue() const { return m_ScaleValue; }

	private:
		void SetTranslate();
		void SetRotate();
		void SetScale();

	private:

		DirectX::XMMATRIX m_Translate;
		DirectX::XMMATRIX m_Rotate;
		DirectX::XMMATRIX m_Scale;

		DirectX::XMFLOAT3 m_TranslateValue;
		DirectX::XMFLOAT3 m_RotateValue;
		DirectX::XMFLOAT3 m_ScaleValue;
	};

}
