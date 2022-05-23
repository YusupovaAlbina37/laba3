#define M_PI 3.14159F
#define ToRadian(x) ((x) * M_PI / 180.0f)
#define ToDegree(x) ((x) * 180.0f / M_PI)
#include "pipeline3.h"

void Pipeline::InitScaleTransform(mat4& m) const
{
	m[0][0] = m_scale.x; m[0][1] = 0.0f;      m[0][2] = 0.0f;      m[0][3] = 0.0f;
	m[1][0] = 0.0f;      m[1][1] = m_scale.y; m[1][2] = 0.0f;      m[1][3] = 0.0f;
	m[2][0] = 0.0f;      m[2][1] = 0.0f;      m[2][2] = m_scale.z; m[2][3] = 0.0f;
	m[3][0] = 0.0f;      m[3][1] = 0.0f;      m[3][2] = 0.0f;      m[3][3] = 1.0f;
}

void Pipeline::InitRotateTransform(mat4& m) const
{
	mat4 rx, ry, rz;

	const float x = ToRadian(m_rotateInfo.x);
	const float y = ToRadian(m_rotateInfo.y);
	const float z = ToRadian(m_rotateInfo.z);

	rx[0][0] = 1.0f; rx[0][1] = 0.0f;    rx[0][2] = 0.0f;     rx[0][3] = 0.0f;
	rx[1][0] = 0.0f; rx[1][1] = cosf(x); rx[1][2] = -sinf(x); rx[1][3] = 0.0f;
	rx[2][0] = 0.0f; rx[2][1] = sinf(x); rx[2][2] = cosf(x);  rx[2][3] = 0.0f;
	rx[3][0] = 0.0f; rx[3][1] = 0.0f;    rx[3][2] = 0.0f;     rx[3][3] = 1.0f;

	ry[0][0] = cosf(y); ry[0][1] = 0.0f; ry[0][2] = -sinf(y); ry[0][3] = 0.0f;
	ry[1][0] = 0.0f;    ry[1][1] = 1.0f; ry[1][2] = 0.0f;     ry[1][3] = 0.0f;
	ry[2][0] = sinf(y); ry[2][1] = 0.0f; ry[2][2] = cosf(y);  ry[2][3] = 0.0f;
	ry[3][0] = 0.0f;    ry[3][1] = 0.0f; ry[3][2] = 0.0f;     ry[3][3] = 1.0f;

	rz[0][0] = cosf(z); rz[0][1] = -sinf(z); rz[0][2] = 0.0f; rz[0][3] = 0.0f;
	rz[1][0] = sinf(z); rz[1][1] = cosf(z);  rz[1][2] = 0.0f; rz[1][3] = 0.0f;
	rz[2][0] = 0.0f;    rz[2][1] = 0.0f;     rz[2][2] = 1.0f; rz[2][3] = 0.0f;
	rz[3][0] = 0.0f;    rz[3][1] = 0.0f;     rz[3][2] = 0.0f; rz[3][3] = 1.0f;

	m = rz * ry * rx; //объединение преобразований
}

void Pipeline::InitTranslationTransform(float x, float y, float z, mat4& m) const
{
	m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
	m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
	m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
	m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;

}

void Pipeline::InitPerspectiveProj(mat4& m) const
{
	const float ar = m_persProj.Width / m_persProj.Height;
	const float zNear = m_persProj.zNear;
	const float zFar = m_persProj.zFar;
	const float zRange = zNear - zFar;
	const float tanHalfFOV = tanf(ToRadian(m_persProj.FOV / 2.0f));

	m[0][0] = 1.0f / (tanHalfFOV * ar); m[0][1] = 0.0f;              m[0][2] = 0.0f;                     m[0][3] = 0.0;
	m[1][0] = 0.0f;                     m[1][1] = 1.0f / tanHalfFOV; m[1][2] = 0.0f;                     m[1][3] = 0.0;
	m[2][0] = 0.0f;                     m[2][1] = 0.0f;              m[2][2] = (-zNear - zFar) / zRange; m[2][3] = 2.0f * zFar*zNear / zRange;
	m[3][0] = 0.0f;                     m[3][1] = 0.0f;              m[3][2] = 1.0f;                     m[3][3] = 0.0;
}

const mat4* Pipeline::GetTrans()
{
	mat4 ScaleTrans, RotateTrans, TranslationTrans, CameraTranslationTrans, CameraRotateTrans, PersProjTrans;

	InitScaleTransform(ScaleTrans);
	InitRotateTransform(RotateTrans);
	InitTranslationTransform(m_worldPos.x, m_worldPos.y, m_worldPos.z, TranslationTrans);
	InitTranslationTransform(-m_camera.Pos.x, -m_camera.Pos.y, -m_camera.Pos.z, CameraTranslationTrans);
	InitCameraTransform(CameraRotateTrans, m_camera.Target, m_camera.Up); 
	InitPerspectiveProj(PersProjTrans);

	m_transformation = CameraRotateTrans * CameraTranslationTrans * TranslationTrans * RotateTrans * ScaleTrans * PersProjTrans;



	return &m_transformation;
}