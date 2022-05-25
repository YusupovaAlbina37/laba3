#include "Camera.h"

const static float StepSize = 0.1f;

bool Camera::OnKeyboard(int Key)
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