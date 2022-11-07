#include <gl\glew.h>
#include <iostream>
#include <fstream>
#include <MeGlWindow.h>
#include <QtGui/qkeyevent>

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
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
struct vec3 {
	float r, g, b;

	vec3() { r = 0; g = 0; b = 0; };
	vec3(float vr, float vg, float vb) {
		r = vr;
		g = vg;
		b = vb;
	}
	vec3 operator += (vec3 vec) {
		vec3 v;
		r = r + vec.r;
		g = g + vec.g;
		b = b + vec.b;
		v.r = r;
		v.g = g;
		v.b = b;

		return v;
	}
};

vec2 offset;
vec2 offsetA;
vec2 offsetB;
vec3 color = vec3(0.05, 0.0, 0.0);
float moveSpeed = 0.01f;

// For WASD input
void MeGlWindow::keyPressEvent(QKeyEvent* e)
{
	switch (e->key())
	{
	case Qt::Key::Key_W:
		offsetA += vec2(0.0f, moveSpeed);
		break;
	case Qt::Key::Key_A:
		offsetA += vec2(-moveSpeed, 0.0f);
		break;
	case Qt::Key::Key_S:
		offsetA += vec2(0.0f, -moveSpeed);
		break;
	case Qt::Key::Key_D:
		offsetA += vec2(moveSpeed, 0.0f);
		break;
	case Qt::Key::Key_Up:
		offsetB += vec2(0.0f, moveSpeed);
		break;
	case Qt::Key::Key_Left:
		offsetB += vec2(-moveSpeed, 0.0f);
		break;
	case Qt::Key::Key_Down:
		offsetB += vec2(0.0f, -moveSpeed);
		break;
	case Qt::Key::Key_Right:
		offsetB += vec2(moveSpeed, 0.0f);
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

const uint NUM_VERTICES_PER_TRI = 3;
const uint NUM_FLOATS_PER_VERTICE = 7; // x,y,r,g,b,offsetX, offsetY
const uint TRIANGLE_BYTE_SIZE = NUM_VERTICES_PER_TRI * NUM_FLOATS_PER_VERTICE * sizeof(float);
const uint NUM_TRI = 7;
const uint TOTAL_IDX_BYTE_SIZE = NUM_TRI * 3 * sizeof(GLushort);


void sendDataToOpenGL()
{
	GLuint myBufferID;

	glGenBuffers(1, &myBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID);
	// NULL: Fill in data later
	glBufferData(GL_ARRAY_BUFFER, NUM_TRI * TRIANGLE_BYTE_SIZE + TOTAL_IDX_BYTE_SIZE, NULL, GL_STATIC_DRAW);

	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 7, 0);
	// Color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (char*)(sizeof(float) * 2));
	// Offset
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (char*)(sizeof(float) * 5));

	// Triangle
	//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), (char*)verts);
	// Index
	GLushort indices[] = { 0, 1, 2, 1, 2, 3, 4, 5, 6, 5, 6, 7, 8, 9, 10, 9, 10, 11, 12, 13, 14 };
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myBufferID);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, NUM_TRI * TRIANGLE_BYTE_SIZE, sizeof(indices), (char*)indices);
}

void UpdateDataToOpenGL() {
	//GLfloat verts[] =
	//{
	//	-1.0f, +0.0f,
	//	r, g, b,
	//	-0.8f, -0.2f,
	//	r, g, b,
	//	+0.6f, +0.0f,
	//	r, g, b,
	//	+0.6f, -0.2f,
	//	r, g, b,
	//	+0.6f, +0.1f,
	//	r, g, b,
	//	+0.6f, -0.3f,
	//	r, g, b,
	//	+0.65f, +0.1f,
	//	r, g, b,
	//	+0.65f, -0.3f,
	//	r, g, b,
	//	+0.65f, +0.0f,
	//	r, g, b,
	//	+0.65f, -0.2f,
	//	r, g, b,
	//	+1.0f, +0.0f,
	//	r, g, b,
	//	+1.0f, -0.2f,
	//	r, g, b,

	//	-0.8f, +0.5f,
	//	r, g, b,
	//	-1.0f, +0.5f,
	//	r, g, b,
	//	-0.6f, +0.8f,
	//	r, g, b,
	//};

	GLfloat verts[] =
	{
		-1.0f, +0.0f, color.r, color.g, color.b, offsetA.x, offsetA.y,
		-0.8f, -0.2f, color.r, color.g, color.b, offsetA.x, offsetA.y,
		+0.6f, +0.0f, color.r, color.g, color.b, offsetA.x, offsetA.y,
		+0.6f, -0.2f,color.r, color.g, color.b, offsetA.x, offsetA.y,
		+0.6f, +0.1f, color.r, color.g, color.b, offsetA.x, offsetA.y,
		+0.6f, -0.3f, color.r, color.g, color.b, offsetA.x, offsetA.y,
		+0.65f, +0.1f, color.r, color.g, color.b, offsetA.x, offsetA.y,
		+0.65f, -0.3f, color.r, color.g, color.b, offsetA.x, offsetA.y,
		+0.65f, +0.0f, color.r, color.g, color.b, offsetA.x, offsetA.y,
		+0.65f, -0.2f, color.r, color.g, color.b, offsetA.x, offsetA.y,
		+1.0f, +0.0f, color.r, color.g, color.b, offsetA.x, offsetA.y,
		+1.0f, -0.2f, color.r, color.g, color.b, offsetA.x, offsetA.y,

		-0.8f, +0.5f, color.r, color.g, color.b, offsetB.x, offsetB.y,
		-1.0f, +0.5f, color.r, color.g, color.b, offsetB.x, offsetB.y,
		-0.6f, +0.8f, color.r, color.g, color.b, offsetB.x, offsetB.y,
	};

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), (char*)verts);
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

// Update when something is changed (e.g. screen size and input)
void MeGlWindow::paintGL()
{
	// Background color
	glClearColor(0.0, 0.15, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();

	// Draw based on screen size
	glViewport(0, 0, width(), height());
	

	UpdateDataToOpenGL();
	glDrawElements(GL_TRIANGLES, 21, GL_UNSIGNED_SHORT, (char*)(NUM_TRI * TRIANGLE_BYTE_SIZE));

	vec2 tempOffsetA = offsetA;
	offsetA += vec2(-0.06f, 0.14f);
	vec2 tempOffsetB = offsetB;
	offsetB += vec2(-0.06f, 0.14f);
	vec3 tempColor = color;
	color = vec3(1.0, 0.0, 0.0);

	UpdateDataToOpenGL();
	glDrawElements(GL_TRIANGLES, 21, GL_UNSIGNED_SHORT, (char*)(NUM_TRI * TRIANGLE_BYTE_SIZE));
	offsetA = tempOffsetA;
	offsetB = tempOffsetB;
	color = tempColor;

	// Follow the vertex sequence to draw triangles (might have two same vertices)
	//glDrawArrays(GL_TRIANGLES, 0, 6);
}