#pragma once
#include <iostream>
#include <Magick++.h>
#include "Camera.h"
#include <Magick++.h>
using namespace Magick;
using namespace std;

class Texture
{
public:
	Texture(GLenum TextureTarget, const string& FileName);

	bool Load();

	void Bind(GLenum TextureUnit);
private:
	string m_fileName; //имя файла текстуры
	GLenum m_textureTarget;
	GLuint m_textureObj;
    Image* m_pImage;
	Blob m_blob; //BLOB (большой бинарный объект) - это полезный механизм для хранения зашифрованного изображения 
	//в память так, что оно может быть использовано сторонними программами
};