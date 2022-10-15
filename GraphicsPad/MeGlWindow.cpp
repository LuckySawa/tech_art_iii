#include <gl\glew.h>
#include <iostream>
#include <fstream>
#include <MeGlWindow.h>
#include <QtGui/qkeyevent>
using namespace std;

struct vec2 {
	float x, y;

	vec2() { x = 0; y = 0; };
	vec2(float vx, float vy) {
		x = vx;
		y = vy;
	}
	vec2 operator += (vec2 vec) {
		vec2 v;
		x = x + vec.x;
		y = y + vec.y;
		v.x = x;
		v.y = y;

		return v;
	}
};

vec2 offset;
vec2 blueOffset;
vec2 redOffset;
float moveSpeed = 0.01f;

// For WASD input
void MeGlWindow::keyPressEvent(QKeyEvent* e)
{
	switch (e->key())
	{
	case Qt::Key::Key_W:
		blueOffset += vec2(0.0f, moveSpeed);
		break;
	case Qt::Key::Key_A:
		blueOffset += vec2(-moveSpeed, 0.0f);
		break;
	case Qt::Key::Key_S:
		blueOffset += vec2(0.0f, -moveSpeed);
		break;
	case Qt::Key::Key_D:
		blueOffset += vec2(moveSpeed, 0.0f);
		break;
	case Qt::Key::Key_Up:
		redOffset += vec2(0.0f, moveSpeed);
		break;
	case Qt::Key::Key_Left:
		redOffset += vec2(-moveSpeed, 0.0f);
		break;
	case Qt::Key::Key_Down:
		redOffset += vec2(0.0f, -moveSpeed);
		break;
	case Qt::Key::Key_Right:
		redOffset += vec2(moveSpeed, 0.0f);
		break;
	}

	repaint();
}


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
		-1.0f + blueOffset.x, +0.0f + blueOffset.y,
		r,g,b,
		-0.8f + blueOffset.x, -0.2f + blueOffset.y,
		r,g,b,
		+0.6f + blueOffset.x, +0.0f + blueOffset.y,
		r,g,b,
		+0.6f + blueOffset.x, -0.2f + blueOffset.y,
		r,g,b,
		+0.6f + blueOffset.x, +0.1f + blueOffset.y,
		r,g,b,
		+0.6f + blueOffset.x, -0.3f + blueOffset.y,
		r,g,b,
		+0.65f + blueOffset.x, +0.1f + blueOffset.y,
		r,g,b,
		+0.65f + blueOffset.x, -0.3f + blueOffset.y,
		r,g,b,
		+0.65f + blueOffset.x, +0.0f + blueOffset.y,
		r,g,b,
		+0.65f + blueOffset.x, -0.2f + blueOffset.y,
		r,g,b,
		+1.0f + blueOffset.x, +0.0f + blueOffset.y,
		r,g,b,
		+1.0f + blueOffset.x, -0.2f + blueOffset.y,
		r,g,b,


		-0.8f + redOffset.x, +0.5f + redOffset.y,
		r,g,b,
		-1.0f + redOffset.x, +0.5f + redOffset.y,
		r,g,b,
		-0.6f + redOffset.x, +0.8f + redOffset.y,
		r,g,b,
	};
	GLuint myBufferID;

	// Create buffer and ref, bind, fill in data
	glGenBuffers(1, &myBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	
	// Position
	glEnableVertexAttribArray(0);
	// (index, size(xy), type, normalized, stride, pointer)
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	// Color
	glEnableVertexAttribArray(1);
	// (index, size(rgb), type, normalized, stride, pointer)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (char*)(sizeof(float) * 2));

	//// Offset
	//glEnableVertexAttribArray(2);
	//// (index, size(xy), type, normalized, stride, pointer)
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (char*)(sizeof(float) * 2));

	// Indices to form triangles
	GLushort indices[] = { 0, 1, 2, 1, 2, 3, 4, 5, 6, 5, 6, 7, 8, 9, 10, 9, 10, 11, 12, 13, 14 };

	// Create buffer and ref, bind, fill in data
	GLuint indexBufferID;
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),
		indices, GL_STATIC_DRAW);
}


string readShaderCode(const char* fileName)
{

	// Need to include fstream
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
	// Handle, like a pointer
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];
	// Can't use .c_str() directly
	string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	// Set shader source to shader handle
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	// Check & Print error
	if( ! checkShaderStatus(vertexShaderID) || ! checkShaderStatus(fragmentShaderID))
		return;

	// Program setup
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	// Check & Print error
	if ( ! checkProgramStatus(programID))
		return;

	glUseProgram(programID);
}

// Overall flow
void MeGlWindow::initializeGL()
{
	glewInit();
	sendDataToOpenGL();
	installShaders();
}

// Update when something is changed (like screen size and input)
void MeGlWindow::paintGL()
{
	sendDataToOpenGL();

	// Background color
	glClearColor(0.0, 0.15, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();

	// Draw based on screen size
	glViewport(0, 0, width(), height());

	// Follow the vertex sequence to draw triangles (might have two same vertices)
	//glDrawArrays(GL_TRIANGLES, 0, 6);

	// Use the indices to draw triangles (no same vertices)
	glDrawElements(GL_TRIANGLES, 21, GL_UNSIGNED_SHORT, 0);

	//glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);
}



