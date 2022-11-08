#include <gl\glew.h>
#include <iostream>
#include <fstream>
#include <MeGlWindow.h>
#include <QtGui/qkeyevent>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Primitives\Vertex.h>
#include <Primitives\ShapeGenerator.h>

using namespace std;
using glm::mat4;
using glm::vec3;

GLuint programID;

#pragma region Input
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
vec2 offsetA;
vec2 offsetB;
vec3 color = vec3(0.05, 0.0, 0.0);
float rotation;
float moveSpeed = 0.01f;
float rotateSpeed = 0.01f;

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

	case Qt::Key::Key_Q:
		rotation += rotateSpeed;
		break;
	case Qt::Key::Key_E:
		rotation -= rotateSpeed;
		break;
	}

	repaint();
}
#pragma endregion

#pragma region Shader
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

string readShaderCode(const char* fileName)
{
	// Need to include fstream
	ifstream meInput(fileName);
	if (!meInput.good())
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
	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	// Program setup
	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	// Check & Print error
	if (!checkProgramStatus(programID))
		return;

	glUseProgram(programID);
}

#pragma endregion




const uint NUM_VERTICES_PER_TRI = 3;
const uint NUM_FLOATS_PER_VERTICE = 7; // x,y,r,g,b,offsetX, offsetY
const uint TRIANGLE_BYTE_SIZE = NUM_VERTICES_PER_TRI * NUM_FLOATS_PER_VERTICE * sizeof(float);
const uint NUM_TRI = 7;
const uint TOTAL_IDX_BYTE_SIZE = NUM_TRI * 3 * sizeof(GLushort);


GLuint indiceNum;

void sendDataToOpenGL()
{
	//glGenBuffers(1, &myBufferID);
	//glBindBuffer(GL_ARRAY_BUFFER, myBufferID);
	//// NULL: Fill in data later
	//glBufferData(GL_ARRAY_BUFFER, NUM_TRI * TRIANGLE_BYTE_SIZE + TOTAL_IDX_BYTE_SIZE, NULL, GL_STATIC_DRAW);

	//// Position
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 7, 0);
	//// Color
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (char*)(sizeof(float) * 2));
	//// Offset
	//glEnableVertexAttribArray(2);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (char*)(sizeof(float) * 5));

	//// Triangle
	////glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), (char*)verts);
	//// Index
	//GLushort indices[] = { 0, 1, 2, 1, 2, 3, 4, 5, 6, 5, 6, 7, 8, 9, 10, 9, 10, 11, 12, 13, 14 };
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myBufferID);
	//glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, NUM_TRI * TRIANGLE_BYTE_SIZE, sizeof(indices), (char*)indices);



	GLuint myBufferID;
	ShapeData shape = ShapeGenerator::makeCube();

	glGenBuffers(1, &myBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID);

	// NULL: Fill in data later
	glBufferData(GL_ARRAY_BUFFER, shape.vertexBufferSize() + shape.indexBufferSize(), shape.vertices, GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, NUM_TRI * TRIANGLE_BYTE_SIZE + TOTAL_IDX_BYTE_SIZE, NULL, GL_STATIC_DRAW);

	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	// Color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (char*)(sizeof(float) * 3));


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myBufferID);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape.indexBufferSize(), shape.indices, GL_STATIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, shape.vertexBufferSize(), shape.indexBufferSize(), (char*)shape.indices);
	//glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, NUM_TRI * TRIANGLE_BYTE_SIZE, sizeof(indices), (char*)indices);
	indiceNum = shape.numIndices;

	shape.cleanup();
}

void UpdateDataToOpenGL() {
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



// Overall flow
void MeGlWindow::initializeGL()
{
	glewInit();
	sendDataToOpenGL();
	installShaders();
}

// Update
void MeGlWindow::paintGL()
{
	//UpdateDataToOpenGL();
	//glDrawElements(GL_TRIANGLES, 21, GL_UNSIGNED_SHORT, (char*)(NUM_TRI * TRIANGLE_BYTE_SIZE));

	//vec2 tempOffsetA = offsetA;
	//offsetA += vec2(-0.06f, 0.14f);
	//vec2 tempOffsetB = offsetB;
	//offsetB += vec2(-0.06f, 0.14f);
	//vec3 tempColor = color;
	//color = vec3(1.0, 0.0, 0.0);

	//UpdateDataToOpenGL();
	//glDrawElements(GL_TRIANGLES, 21, GL_UNSIGNED_SHORT, (char*)(NUM_TRI * TRIANGLE_BYTE_SIZE));
	//offsetA = tempOffsetA;
	//offsetB = tempOffsetB;
	//color = tempColor;

	// Follow the vertex sequence to draw triangles (might have two same vertices)
	//glDrawArrays(GL_TRIANGLES, 0, 6);





	// Background color
	glClearColor(0.0, 0.15, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();

	// Draw based on screen size
	glViewport(0, 0, width(), height());

	// Matrix calculation
	mat4 translationMatrix = glm::translate(mat4(), vec3(0.0f, 0.0f, -5.0f));
	mat4 rotationMatrix = glm::rotate(mat4(), 54.0f + rotation, vec3(1.0f, 0.0f, 0.0f));
	mat4 projectionMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 10.0f);

	mat4 myTransformMatrix = projectionMatrix * translationMatrix * rotationMatrix;

	GLint myTransformMatrixUniformLocation =
		glGetUniformLocation(programID, "myTransformMatrix");

	glUniformMatrix4fv(myTransformMatrixUniformLocation, 1, GL_FALSE, &myTransformMatrix[0][0]);

	// Update frame
	glDrawElements(GL_TRIANGLES, indiceNum, GL_UNSIGNED_SHORT, (char*)indiceNum);
}