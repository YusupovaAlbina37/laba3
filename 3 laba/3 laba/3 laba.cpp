#include <stdio.h>
#include <iostream>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <Magick++.h>
#include "pipeline3.h"
#include "Camera.h"
#include "Texture.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

struct Vertex
{
	vec3 m_pos;
	vec2 m_tex;

	Vertex() {}

	Vertex(vec3 pos, vec2 tex)
	{
		m_pos = pos;
		m_tex = tex;
	}
};

using namespace glm;

GLuint VBO;
GLuint IBO;
//GLuint gWorldLocation;
GLuint gWVPLocation;
GLuint gSampler;
Texture* pTexture = NULL;
//Camera GameCamera;
Camera* pGameCamera = NULL;

//вершинный шейдер
static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
layout (location = 1) in vec2 TexCoord;                                             \n\
                                                                                    \n\
uniform mat4 gWVP;                                                                  \n\
                                                                                    \n\
out vec2 TexCoord0;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
     gl_Position = gWVP * vec4(Position, 1.0);                                      \n\
     TexCoord0 = TexCoord;                                                          \n\
}";

//пиксельный шейдер
static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
in vec2 TexCoord0;                                                                  \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
uniform sampler2D gSampler;                                                         \n\
void main()                                                                         \n\
{                                                                                   \n\
     FragColor = texture2D(gSampler, TexCoord0.st);                                 \n\
}";

void RenderSceneCB()
{
	pGameCamera->OnRender();

	glClear(GL_COLOR_BUFFER_BIT);

	static float Scale = 0.0f;

	Scale += 0.01f; 

	Pipeline p;
	//p.Rotate(0.0f, Scale, 0.0f);
	p.WorldPos(0.0f, 0.0f, 3.0f);
	p.SetCamera(pGameCamera->GetPos(), pGameCamera->GetTarget(), pGameCamera->GetUp());
	p.SetPerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f);

	glUniformMatrix4fv(gWVPLocation, 1, GL_TRUE, (const GLfloat*)p.GetTrans());

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	pTexture->Bind(GL_TEXTURE0);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glutSwapBuffers();
}

static void SpecialKeyboardCB(int Key, int x, int y) 
{
	pGameCamera->OnKeyboard(Key);
}

static void KeyboardCB(unsigned char Key, int x, int y)
{
	switch (Key) {
	case 'q':
		exit(0);
	}
}

static void PassiveMouseCB(int x, int y)
{
	pGameCamera->OnMouse(x, y);
}

static void InitializeGlutCallbacks()
{
	glutDisplayFunc(RenderSceneCB);
	glutIdleFunc(RenderSceneCB);
	glutSpecialFunc(SpecialKeyboardCB);
	glutPassiveMotionFunc(PassiveMouseCB);
	glutKeyboardFunc(KeyboardCB);
}

static void CreateVertexBuffer()
{
	/*vec3 Vertices[4];
	Vertices[0] = vec3(-1.0f, -1.0f, 0.0f);
	Vertices[1] = vec3(0.0f, -1.0f, 1.0f);
	Vertices[2] = vec3(1.0f, -1.0f, 0.0f);
	Vertices[3] = vec3(0.0f, 1.0f, 0.0f); //вершина пирамиды*/
	/*Vertices[0] = vec3(-1.0f, -1.0f, 0.4f);
	Vertices[1] = vec3(-0.35f, -0.75f, 1.0f);
	Vertices[2] = vec3(1.0f, -1.0f, 0.4f);
	Vertices[3] = vec3(0.0f, 1.0f, 0.0f);*/

	Vertex Vertices[4] = {
	Vertex(vec3(-1.0f, -1.0f, 0.5f), vec2(0.0f, 0.0f)),
	Vertex(vec3(0.0f, -1.0f, -1.0f), vec2(0.5f, 0.0f)),
	Vertex(vec3(1.0f, -1.0f, 0.5f),  vec2(1.0f, 0.0f)),
	Vertex(vec3(0.0f, 1.0f, 0.0f),   vec2(0.5f, 1.0f)) };

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

static void CreateIndexBuffer()
{
	unsigned int Indices[] = { 0, 3, 1,
							   1, 3, 2,
							   2, 3, 0,
							   0, 2, 1 };
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	glShaderSource(ShaderObj, 1, p, Lengths);

	glCompileShader(ShaderObj);

	GLint success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		exit(1);
	}

	glAttachShader(ShaderProgram, ShaderObj);
}

static void CompileShaders()
{
	GLuint ShaderProgram = glCreateProgram();

	if (ShaderProgram == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}
	AddShader(ShaderProgram, pVS, GL_VERTEX_SHADER);
	AddShader(ShaderProgram, pFS, GL_FRAGMENT_SHADER);

	glLinkProgram(ShaderProgram);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	glValidateProgram(ShaderProgram);

	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	glUseProgram(ShaderProgram);

	/*gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
	assert(gWorldLocation != 0xFFFFFFFF);*/
	gWVPLocation = glGetUniformLocation(ShaderProgram, "gWVP");
	assert(gWVPLocation != 0xFFFFFFFF);
	gSampler = glGetUniformLocation(ShaderProgram, "gSampler");
	assert(gSampler != 0xFFFFFFFF);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Tutorial 16");
	glutGameModeString("1280x1024@32");
	glutEnterGameMode();

	InitializeGlutCallbacks();

	pGameCamera = new Camera(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	CreateVertexBuffer();
	CreateIndexBuffer();

	CompileShaders();

	glUniform1i(gSampler, 0); //устанавливаем индексы модулей текстуры, который мы собираемся использовать 
	//внутри сэмплера uniform-переменной в шейдере. 'gSampler' это переменная, значение которой было задано
	//ранее через glGetUniformLocation()
	pTexture = new Texture(GL_TEXTURE_2D, "C://test.png"); //создаем объект Текстуры и загружаем его

	if (!pTexture->Load()) {
		return 1;
	}

	glutMainLoop();

	return 0;
}