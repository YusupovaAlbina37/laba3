#include "Texture.h"

Texture::Texture(GLenum TextureTarget, const string& FileName)
{
	m_textureTarget = TextureTarget;
	m_fileName = FileName;
	m_pImage = NULL;
}
bool Texture::Load() //может вернуть код ошибки если, например, файл не существует, или если ImageMagick получит другие виды ошибок
{
	try
	{
		m_pImage = new Image(m_fileName);//инициализация свойства класса типа Magic::Image используя имя файла текстуры
		//Этот вызов загружает текстуру в память, которая задана private и не может быть напрямую использована OpenGL
		m_pImage->write(&m_blob, "RGBA"); //записываем изображение в объект Magick::Blob используя формат RGBA (красный, зеленый, синий и альфа канал)
	}
	catch (Error& Error) 
	{
		cout << "Error loading texture '" << m_fileName << "': " << Error.what() << endl;
		return false;
	}
	glGenTextures(1, &m_textureObj); //Она генерирует указанное число объектов текстур и помещает их в указатель 
	//на массив GLuint(второй параметр).В нашем случае нам потребуется только 1 объект.
	glBindTexture(m_textureTarget, m_textureObj); //OpenGL должен знать, с каким объектом текстур работать. Эта цель функции glBindTexture(). 
	//Она сообщает OpenGL объект текстуры, который относится ко всем вызовам, 
	//связанным с текстурами, до тех пор, пока новый объект текстур не будет передан
	//В дополнении к указателю (второй параметр) мы также указываем позицию текстуры

	glTexImage2D(m_textureTarget, 0, GL_RGBA, m_pImage->columns(), m_pImage->rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_blob.data()); 
	//функция для загрузки главной части объекта текстуры, что по сути, сами данные текстуры
	//(позиция, уровень детализации (LOD), внутренний формат (в нём OpenGL хранит текстуру), ширина, высота текстуры в текселях),рамка,
	//формат(кол-во каналов), тип, адрес в памяти)

	glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//здесь мы указываем фильтры, которые будут использованы для увеличения и минимализации

}
void Texture::Bind(GLenum TextureUnit) //привязываем объект текстур к модулю
{
	glActiveTexture(TextureUnit);
	glBindTexture(m_textureTarget, m_textureObj);
}