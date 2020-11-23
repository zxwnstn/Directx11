#pragma once

#include "Util/MathType.h"

namespace Engine {

	namespace CBuffer{}

	class Transform
	{
	public:
		Transform();
		Transform(const vec3& translate, const vec3& rotate, const vec3& scale);

		void SetTranslate(float x, float y, float z);
		void SetTranslate(const vec3& translate);
		void SetRotate(float x, float y, float z);
		void SetRotate(const vec3& rotate);
		void SetScale(float x, float y, float z);
		void SetScale(const vec3& scale);

		void AddTranslate(float x, float y, float z);
		void AddTranslate(const vec3& position);
		void AddRotate(float x, float y, float z);
		void AddRotate(const vec3& rotation);
		void AddScale(float x, float y, float z);
		void AddScale(const vec3& scale);

		void MoveForwad(float d);
		void MoveBack(float d);

		inline const vec3& GetTranslate() const { return m_Translate; }
		inline const vec3& GetRotate() const { return m_Rotate; }
		inline const vec3& GetScale() const { return m_Scale; }
		inline vec3& GetTranslate() { return m_Translate; }
		inline vec3& GetRotate() { return m_Rotate; }
		inline vec3& GetScale() { return m_Scale; }

		vec3 GetLookAtVector() const;

	private:
		bool isChanged = false;

		vec3 m_Translate;
		vec3 m_Rotate;
		vec3 m_Scale;
	};

}
