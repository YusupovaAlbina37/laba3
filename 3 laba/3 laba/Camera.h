#pragma once
#include "pipeline3.h"

class Camera
{
public:
	Camera()
	{
		m_pos = vec3(0.0f, 0.0f, 0.0f);
		m_target = vec3(0.0f, 0.0f, 1.0f);
		m_up = vec3(0.0f, 1.0f, 0.0f);
	}
	Camera(const vec3& Pos, const vec3& Target, const vec3& Up)
	{
		m_pos = Pos; //позиция камеры
		m_target = Target; //направление камеры
		normalize(m_target);
		m_up = Up; //верхний вектор
		normalize(m_up);
	}
	bool OnKeyboard(int Key);
	const vec3& GetPos() const
	{
		return m_pos;
	}
	const vec3& GetTarget() const
	{
		return m_target;
	}
	const vec3& GetUp() const
	{
		return m_up;
	}
	
private:
	vec3 m_pos;
	vec3 m_target;
	vec3 m_up;
};