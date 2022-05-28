#include <GL/glew.h>
#include <GL/freeglut.h>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
//#include <glm/gtx/vector_angle.hpp>

#include "Camera.h"

const static float StepSize = 0.1f; //шаг
const static int MARGIN = 10; //граница

Camera::Camera(int WindowWidth, int WindowHeight)
{
	m_windowWidth = WindowWidth;
	m_windowHeight = WindowHeight;
	m_pos = vec3(0.0f, 0.0f, 0.0f);
	m_target = vec3(1.0f, 0.0f, 0.0f);
	normalize(m_target);
	m_up = vec3(0.0f, 1.0f, 0.0f);
	Init();
}

bool Camera::OnKeyboard(int Key) //функция для перемещения камеры с помощью клавиатуры
{
	bool Ret = false;
	switch (Key)
	{
	    case GLUT_KEY_UP:
		{
			m_pos += (m_target * StepSize);
			Ret = true;
		} break;
	    case GLUT_KEY_DOWN:
		{
			m_pos -= (m_target * StepSize);
			Ret = true;
		} break;
	    case GLUT_KEY_LEFT: 
		{
			vec3 Left = cross(m_target, m_up);
			normalize(Left);
			Left *= StepSize;
			m_pos += Left;
			Ret = true;
		} break;
	    case GLUT_KEY_RIGHT: 
		{
			vec3 Right = cross(m_up, m_target);
			normalize(Right);
			Right *= StepSize;
			m_pos += Right;
			Ret = true;
		} break;
	}
	return Ret;
}

void Camera::Init() //функция, устанавливающая внутренние параметры камеры
{
	vec3 HTarget(m_target.x, 0.0, m_target.z); //направление по горизонтали
	//является проекцией исходного вектора направления на плоскость XZ
	
	normalize(HTarget); //нормирование данного вектора

	if (HTarget.z >= 0.0f) {
		if (HTarget.x >= 0.0f) {
			m_AngleH = 360.0f - degrees(asin(HTarget.z));
		}
		else {
			m_AngleH = 180.0f + degrees(asin(HTarget.z));
		}
	}
	else {
		if (HTarget.x >= 0.0f) {
			m_AngleH = degrees(asin(-HTarget.z));
		}
		else {
			m_AngleH = 90.0f + degrees(asin(-HTarget.z));
		}
	}

	m_AngleV = -degrees(asin(m_target.y)); //вертикальный угол

	m_OnUpperEdge = false; //флаг для проверки не касается ли курсор верхней границы экрана
	m_OnLowerEdge = false; //флаг для проверки не касается ли курсор нижней границы экрана
	m_OnLeftEdge = false; //флаг для проверки не касается ли курсор левой границы экрана
	m_OnRightEdge = false; //флаг для проверки не касается ли курсор правой границы экрана
	m_mousePos.x = m_windowWidth/2;
	m_mousePos.y = m_windowHeight/2;

	glutWarpPointer(m_mousePos.x, m_mousePos.y); //перемещает курсор в центр экрана
}

void Camera::OnMouse(int x, int y) //функция исп-я для того чтобы сообщить, что положение мыши изменилось
{
	//подсчёт разницы между новыми коор-ми и предыдущими по осям X и Y
	const int DeltaX = x - m_mousePos.x; 
	const int DeltaY = y - m_mousePos.y;

	//записываем новые значения для следующих вызовов функции
	m_mousePos.x = x;
	m_mousePos.y = y;

	//обновляем текущие горизонтальные и вертикальные углы на эту разность в значениях
	m_AngleH += (float)DeltaX / 20.0f;
	m_AngleV += (float)DeltaY / 20.0f;

	if (DeltaX == 0) 
	{
		if (x <= MARGIN) //если по коор-те X мышь близка к левому краю
		{
			m_OnLeftEdge = true;
		}
		else if (x >= (m_windowWidth - MARGIN)) //если по коор-те X мышь близка к правому краю
		{
			m_OnRightEdge = true;
		}
	}
	else 
	{
		m_OnLeftEdge = false;
		m_OnRightEdge = false;
	}

	if (DeltaY == 0) 
	{
		if (y <= MARGIN) //если по коор-те Y мышь близка к верхнему краю
		{
			m_OnUpperEdge = true;
		}
		else if (y >= (m_windowHeight - MARGIN)) //если по коор-те Y мышь близка к нижнему краю 
		{
			m_OnLowerEdge = true;
		}
	}
	else 
	{
		m_OnUpperEdge = false;
		m_OnLowerEdge = false;
	}

	Update(); //вызываем для перерасчёта векторов направления и вектора вверх, 
	//основанных на новых горизонтальном и вертикальном углах
}
void Camera::OnRender() //ф-я, которая необходима для случаев, когда мышь не движется, но находится около одной из границ экрана
{
	bool ShouldUpdate = false;

	if (m_OnLeftEdge) 
	{
		m_AngleH -= 0.1f;
		ShouldUpdate = true;
	}
	else if (m_OnRightEdge) 
	{
		m_AngleH += 0.1f;
		ShouldUpdate = true;
	}

	if (m_OnUpperEdge) 
	{
		if (m_AngleV > -90.0f) 
		{
			m_AngleV -= 0.1f;
			ShouldUpdate = true;
		}
	}
	else if (m_OnLowerEdge) 
	{
		if (m_AngleV < 90.0f) 
		{
			m_AngleV += 0.1f;
			ShouldUpdate = true;
		}
	}

	if (ShouldUpdate) 
	{
		Update();
	}
}

void Camera::Update() //функция обновляет значения векторов направления и вверх согласно горизонтальному и вертикальному углам
{
	const vec3 Vaxis(0.0f, 1.0f, 0.0f);//вертикальная ось

	vec3 View(1.0f, 0.0f, 0.0f); //вектор обзора
	
	View = rotate(View, radians(m_AngleH), Vaxis);
	normalize(View);

	vec3 Haxis = cross(Vaxis, View); //новая горизонтальная ось
	normalize(Haxis);

	View = rotate(View, radians(m_AngleV), Haxis);
	normalize(View);

	m_target = normalize(View);
	m_up = normalize(cross(m_target, Haxis));
}