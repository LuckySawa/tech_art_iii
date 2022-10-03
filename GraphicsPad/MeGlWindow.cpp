#include <gl\glew.h>
#include <iostream>
#include <fstream>
#include <MeGlWindow.h>
using namespace std;

// Check & Print error
bool checkStatus(
	GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		cout << buffer << endl;
		delete[] buffer;
		return false;
	}
	return true;
}

bool checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID)
{
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

void sendDataToOpenGL()
{
	
	GLfloat r = 1.0;
	GLfloat g = 0.0;
	GLfloat b = 0.0;

	GLfloat verts[] =
	{
		//+0.0f, +1.0f,
		//+1.0f, +0.0f, +0.0f,
		//-1.0f, -1.0f,
		//+0.0f, +1.0f, +0.0f,
		//+1.0f, -1.0f,
		//+0.0f, +0.0f, +1.0f,

		-1.0f, +0.0f,
		r,g,b,
		-0.8f, -0.2f,
		r,g,b,
		+0.6f, +0.0f,
		r,g,b,
		+0.6f, -0.2f,
		r,g,b,
		+0.6f, +0.1f,
		r,g,b,
		+0.6f, -0.3f,
		r,g,b,
		+0.65f, +0.1f,
		r,g,b,
		+0.65f, -0.3f,
		r,g,b,
		+0.65f, +0.0f,
		r,g,b,
		+0.65f, -0.2f,
		r,g,b,
		+1.0f, +0.0f,
		r,g,b,
		+1.0f, -0.2f,
		r,g,b,
	};
	GLuint myBufferID;

	// Create buffer and ref, bind, fill in data
	glGenBuffers(1, &myBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts),
		verts, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	// (index, size(xyzw), type, normalized, stride, pointer)
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (char*)(sizeof(float) * 2));

	// Indices to form triangles
	GLushort indices[] = { 0, 1, 2, 1, 2, 3, 4, 5, 6, 5, 6, 7, 8, 9, 10, 9, 10, 11 };

	// Create buffer and ref, bind, fill in data
	GLuint indexBufferID;
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),
		indices, GL_STATIC_DRAW);
}

string readShaderCode(const char* fileName)
{
	ifstream meInput(fileName);
	if ( ! meInput.good())
	{
		cout << "File failed to load..." << fileName;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>());
}

void installShaders()
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];
	string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if( ! checkShaderStatus(vertexShaderID) || ! checkShaderStatus(fragmentShaderID))
		return;

	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	if ( ! checkProgramStatus(programID))
		return;

	glUseProgram(programID);
}

void MeGlWindow::initializeGL()
{
	glewInit();
	sendDataToOpenGL();
	installShaders();
}

// Update every frame
void MeGlWindow::paintGL()
{
	// Background color
	glClearColor(0.2, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();

	// Draw based on screen size
	glViewport(0, 0, width(), height());

	// Follow the vertex sequence to draw triangles (might have two same vertices)
	//glDrawArrays(GL_TRIANGLES, 0, 6);

	// Use the indices to draw triangles (no same vertices)
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);
}