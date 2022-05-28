#pragma once
#include "pipeline3.h"

class Camera
{
public:
	Camera(int WindowWidth, int WindowHeight);
	Camera(int WindowWidth, int WindowHeight, const vec3& Pos, const vec3& Target, const vec3& Up)
	{
		m_windowWidth = WindowWidth;
		m_windowHeight = WindowHeight;
		m_pos = Pos; //позиция камеры
		m_target = Target; //направление камеры
		normalize(m_target);
		m_up = Up; //верхний вектор
		normalize(m_up);

		Init();

	}
	bool OnKeyboard(int Key);
	void OnMouse(int x, int y);
	void OnRender();

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

	void Init();
	void Update();

	vec3 m_pos; //вектор позиции
	vec3 m_target; //вектор направления
	vec3 m_up; //верхний вектор

	int m_windowWidth; //ширина
	int m_windowHeight; //высота
	
	float m_AngleH; //горизонтальный угол
	float m_AngleV; // вертикальный угол

	bool m_OnUpperEdge; //флаг для проверки не касается ли курсор верхней границы экрана
	bool m_OnLowerEdge; //флаг для проверки не касается ли курсор нижней границы экрана
	bool m_OnLeftEdge; //флаг для проверки не касается ли курсор левой границы экрана
	bool m_OnRightEdge; //флаг для проверки не касается ли курсор правой границы экрана

	ivec2 m_mousePos; //позиция курсора

};