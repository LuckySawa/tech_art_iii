#include <gl\glew.h>
#include <iostream>
#include <fstream>
#include <QtGui\qmouseevent>
#include <QtGui\qkeyevent>
#include <MeGlWindow.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\transform.hpp>
#include <Primitives\Vertex.h>
#include <Primitives\ShapeGenerator.h>
#include "Camera.h"

using namespace std;
using glm::vec3;
using glm::vec4;
using glm::mat4;

const uint NUM_VERTICES_PER_TRI = 3;
const uint NUM_FLOATS_PER_VERTICE = 12;
const uint VERTEX_BYTE_SIZE = NUM_FLOATS_PER_VERTICE * sizeof(float);


GLuint programID;
GLuint passThroughProgramID;
GLuint textureProgramID;

Camera camera;
GLuint theBufferID;


GLuint planeNumIndices;
GLuint torusNumIndices;
GLuint arrowNumIndices;
GLuint cubeNumIndices;


GLuint planeVertexArrayObjectID;
GLuint torusVertexArrayObjectID;
GLuint arrowVertexArrayObjectID;
GLuint cubeVertexArrayObjectID;


GLuint planeIndexByteOffset;
GLuint torusIndexByteOffset;
GLuint arrowIndexByteOffset;
GLuint cubeIndexByteOffset;

vec3 lightPos = vec3(0.0, 3.0, 0.0);

void FillInBuffer(ShapeData shape, GLsizeiptr* currentOffsetOut, GLuint* indexByteOffsetOut) {

	GLsizeiptr currentOffset = *currentOffsetOut;
	GLuint indexByteOffset = 0;

	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, shape.vertexBufferSize(), shape.vertices);
	currentOffset += shape.vertexBufferSize();
	indexByteOffset = currentOffset;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, shape.indexBufferSize(), shape.indices);
	currentOffset += shape.indexBufferSize();
	
	*currentOffsetOut = currentOffset;
	*indexByteOffsetOut = indexByteOffset;
}

void SetVertexAttributePointer(GLuint VAOID, GLuint offset) {

	glBindVertexArray(VAOID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, theBufferID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)offset);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(offset + sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(offset + sizeof(float) * 6));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(offset + sizeof(float) * 9));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theBufferID);

}

void TransformShape(GLuint VAO, mat4 worldToProjectionMatrix, mat4 modelToWorldMatrix, GLuint indexByteOffset, GLuint numIndices,
	GLuint fullTransformationUniformLocation, GLuint modelToWorldMatrixUniformLocation) {

	glBindVertexArray(VAO);
	mat4 modelToProjectionMatrix = worldToProjectionMatrix * modelToWorldMatrix;
	glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &modelToWorldMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, (void*)indexByteOffset);

}

void LoadTexture() {
	const char* texName = "texture/normal.png";
	QImage timg = QGLWidget::convertToGLFormat(QImage(texName, "PNG"));
	// Copy file to OpenGL 
	glActiveTexture(GL_TEXTURE0);
	GLuint tid;
	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, timg.width(), timg.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, timg.bits());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Set the Tex1 sampler uniform to refer to texture unit 0 
	int loc = glGetUniformLocation(programID, "Tex1");
	if (loc >= 0) glUniform1i(loc, 0); else fprintf(stderr, "Uniform variable Tex1 not found!\n");
}
void MeGlWindow::sendDataToOpenGL()
{
	ShapeData plane = ShapeGenerator::makePlane();
	ShapeData torus = ShapeGenerator::makeTorus();
	ShapeData arrow = ShapeGenerator::makeArrow();
	ShapeData cube = ShapeGenerator::makeCube();

	glGenBuffers(1, &theBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, theBufferID);
	glBufferData(GL_ARRAY_BUFFER, 
		plane.vertexBufferSize() + plane.indexBufferSize() +
		torus.vertexBufferSize() + torus.indexBufferSize() + 
		arrow.vertexBufferSize() + arrow.indexBufferSize() +
		cube.vertexBufferSize() + cube.indexBufferSize(),
		0, GL_STATIC_DRAW);

	
	GLsizeiptr currentOffset = 0;

	FillInBuffer(plane, &currentOffset, &planeIndexByteOffset);
	FillInBuffer(torus, &currentOffset, &torusIndexByteOffset);
	FillInBuffer(arrow, &currentOffset, &arrowIndexByteOffset);
	FillInBuffer(cube, &currentOffset, &cubeIndexByteOffset);


	planeNumIndices = plane.numIndices;
	torusNumIndices = torus.numIndices;
	arrowNumIndices = arrow.numIndices;
	cubeNumIndices = cube.numIndices;

	// Generate VAO
	glGenVertexArrays(1, &planeVertexArrayObjectID);
	glGenVertexArrays(1, &torusVertexArrayObjectID);
	glGenVertexArrays(1, &arrowVertexArrayObjectID);
	glGenVertexArrays(1, &cubeVertexArrayObjectID);

	// Set vertex attribute pointer
	SetVertexAttributePointer(planeVertexArrayObjectID, 0);
	GLuint torusByteOffset = plane.vertexBufferSize() + plane.indexBufferSize();
	SetVertexAttributePointer(torusVertexArrayObjectID, torusByteOffset);
	GLuint arrowByteOffset = torusByteOffset + torus.vertexBufferSize() + torus.indexBufferSize();
	SetVertexAttributePointer(arrowVertexArrayObjectID, arrowByteOffset);
	GLuint cubeByteOffset = arrowByteOffset + arrow.vertexBufferSize() + arrow.indexBufferSize();
	SetVertexAttributePointer(cubeVertexArrayObjectID, cubeByteOffset);

	plane.cleanup();
	torus.cleanup();
	arrow.cleanup();
	cube.cleanup();
}

void MeGlWindow::paintGL()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width(), height());

	mat4 modelToProjectionMatrix;
	mat4 worldToViewMatrix = camera.getWorldToViewMatrix();
	mat4 viewToProjectionMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 20.0f);
	mat4 worldToProjectionMatrix = viewToProjectionMatrix * worldToViewMatrix;

	GLuint fullTransformationUniformLocation;
	GLuint modelToWorldMatrixUniformLocation;

	glm::vec3 lightPositionWorld = lightPos;

	glUseProgram(programID);

	vec4 ambientLight(0.1f, 0.05f, 0.0f, 1.0f);
	glm::vec3 eyePosition = camera.getPosition();

	// UniformLoc for lighting calculation
	glUniform4fv(glGetUniformLocation(programID, "ambientLight"), 1, &ambientLight[0]);
	glUniform3fv(glGetUniformLocation(programID, "eyePositionWorld"), 1, &eyePosition[0]);
	glUniform3fv(glGetUniformLocation(programID, "lightPositionWorld"), 1, &lightPositionWorld[0]);
	// UniformLoc for transform
	fullTransformationUniformLocation = glGetUniformLocation(programID, "modelToProjectionMatrix");
	modelToWorldMatrixUniformLocation = glGetUniformLocation(programID, "modelToWorldMatrix");


	// A plane with no transformations
	mat4 planeModelToWorldMatrix;
	TransformShape(planeVertexArrayObjectID, worldToProjectionMatrix, planeModelToWorldMatrix, planeIndexByteOffset, planeNumIndices,
		fullTransformationUniformLocation, modelToWorldMatrixUniformLocation);

	// Three spheres, different positions and scales
	mat4 torusModelToWorldMatrix = glm::translate(2.0f, 0.5f, 0.0f);
	TransformShape(torusVertexArrayObjectID, worldToProjectionMatrix, torusModelToWorldMatrix, torusIndexByteOffset, torusNumIndices,
		fullTransformationUniformLocation, modelToWorldMatrixUniformLocation);

	torusModelToWorldMatrix = glm::scale(glm::translate(2.0f, 0.5f, 4.0f), glm::vec3(2.0f, 1.0f, 1.0f));
	TransformShape(torusVertexArrayObjectID, worldToProjectionMatrix, torusModelToWorldMatrix, torusIndexByteOffset, torusNumIndices,
		fullTransformationUniformLocation, modelToWorldMatrixUniformLocation);
	
	torusModelToWorldMatrix = glm::scale(glm::translate(2.0f, 0.5f, 8.0f), glm::vec3(1.0f, 0.2f, 1.0f)); 
	TransformShape(torusVertexArrayObjectID, worldToProjectionMatrix, torusModelToWorldMatrix, torusIndexByteOffset, torusNumIndices,
		fullTransformationUniformLocation, modelToWorldMatrixUniformLocation);

	// Two arrows different rotations and scales
	mat4 arrowModelToWorldMatrix = glm::translate(0.0f, 1.0f, 1.2f);
	TransformShape(arrowVertexArrayObjectID, worldToProjectionMatrix, arrowModelToWorldMatrix, arrowIndexByteOffset, arrowNumIndices,
		fullTransformationUniformLocation, modelToWorldMatrixUniformLocation);

	//arrowModelToWorldMatrix = glm::translate(0.0f, 0.0f, 0.0f);
	arrowModelToWorldMatrix = glm::rotate(arrowModelToWorldMatrix, 45.0f, glm::vec3(0.0, 1.0, 0.0));
	arrowModelToWorldMatrix = glm::scale(arrowModelToWorldMatrix, glm::vec3(1.0, 1.0, 2.0));
	//arrowModelToWorldMatrix = glm::translate(arrowModelToWorldMatrix, 0.0f, 1.0f, 1.2f);
	
	TransformShape(arrowVertexArrayObjectID, worldToProjectionMatrix, arrowModelToWorldMatrix, arrowIndexByteOffset, arrowNumIndices,
		fullTransformationUniformLocation, modelToWorldMatrixUniformLocation);

	// Two cubes different rotations and scales.
	mat4 cubeModelToWorldMatrix = glm::translate(-3.0f, 0.2f, -1.0f);
	TransformShape(cubeVertexArrayObjectID, worldToProjectionMatrix, cubeModelToWorldMatrix, cubeIndexByteOffset, cubeNumIndices,
		fullTransformationUniformLocation, modelToWorldMatrixUniformLocation);

	cubeModelToWorldMatrix = glm::rotate(cubeModelToWorldMatrix, glm::radians(1530.0f), glm::vec3(0.0, 1.0, 0.0));
	cubeModelToWorldMatrix = glm::scale(cubeModelToWorldMatrix, glm::vec3(0.7,0.5,2.0));
	TransformShape(cubeVertexArrayObjectID, worldToProjectionMatrix, cubeModelToWorldMatrix, cubeIndexByteOffset, cubeNumIndices,
		fullTransformationUniformLocation, modelToWorldMatrixUniformLocation);

	// Textured plane
	glBindVertexArray(planeVertexArrayObjectID);
	glUseProgram(textureProgramID);
	glUniform4fv(glGetUniformLocation(programID, "ambientLight"), 1, &ambientLight[0]);
	glUniform3fv(glGetUniformLocation(programID, "eyePositionWorld"), 1, &eyePosition[0]);
	glUniform3fv(glGetUniformLocation(programID, "lightPositionWorld"), 1, &lightPositionWorld[0]);
	fullTransformationUniformLocation = glGetUniformLocation(textureProgramID, "modelToProjectionMatrix");
	modelToWorldMatrixUniformLocation = glGetUniformLocation(textureProgramID, "modelToWorldMatrix");
	planeModelToWorldMatrix = glm::translate(10.0f, 0.0f, 0.0f);
	modelToProjectionMatrix = worldToProjectionMatrix * planeModelToWorldMatrix;
	glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
	glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &planeModelToWorldMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, planeNumIndices, GL_UNSIGNED_SHORT, (void*)planeIndexByteOffset);


	// Pass though shader
	glUseProgram(passThroughProgramID);
	fullTransformationUniformLocation = glGetUniformLocation(passThroughProgramID, "modelToProjectionMatrix");
	modelToWorldMatrixUniformLocation = glGetUniformLocation(passThroughProgramID, "modelToWorldMatrix");
	cubeModelToWorldMatrix = glm::translate(lightPositionWorld);
	cubeModelToWorldMatrix = glm::scale(cubeModelToWorldMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(glGetUniformLocation(passThroughProgramID, "modelToProjectionMatrix"), 1, GL_FALSE, &cubeModelToWorldMatrix[0][0]);
	TransformShape(cubeVertexArrayObjectID, worldToProjectionMatrix, cubeModelToWorldMatrix, cubeIndexByteOffset, cubeNumIndices,
		fullTransformationUniformLocation, modelToWorldMatrixUniformLocation);
}

void MeGlWindow::mouseMoveEvent(QMouseEvent* e)
{
	camera.mouseUpdate(glm::vec2(e->x(), e->y()));
	repaint();
}

void MeGlWindow::keyPressEvent(QKeyEvent* e)
{
	switch (e->key())
	{
	case Qt::Key::Key_W:
		camera.moveForward();
		break;
	case Qt::Key::Key_S:
		camera.moveBackward();
		break;
	case Qt::Key::Key_A:
		camera.strafeLeft();
		break;
	case Qt::Key::Key_D:
		camera.strafeRight();
		break;
	case Qt::Key::Key_R:
		camera.moveUp();
		break;
	case Qt::Key::Key_F:
		camera.moveDown();
		break;
	case Qt::Key::Key_Left:
		lightPos.x -= 0.1;
		break;
	case Qt::Key::Key_Right:
		lightPos.x += 0.1;
		break;
	case Qt::Key::Key_Up:
		lightPos.z -= 0.1;
		break;
	case Qt::Key::Key_Down:
		lightPos.z += 0.1;
		break;
	}
	repaint();
}

bool MeGlWindow::checkStatus(
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

bool MeGlWindow::checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool MeGlWindow::checkProgramStatus(GLuint programID)
{
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

string MeGlWindow::readShaderCode(const char* fileName)
{
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

void MeGlWindow::installShaders()
{
	// normal shaders
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

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);

	glLinkProgram(programID);

	if (!checkProgramStatus(programID))
		return;

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	// pass through shaders
	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	temp = readShaderCode("VertexShaderPassThroughCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("FragmentShaderPassThroughCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	passThroughProgramID = glCreateProgram();
	glAttachShader(passThroughProgramID, vertexShaderID);
	glAttachShader(passThroughProgramID, fragmentShaderID);

	glLinkProgram(passThroughProgramID);

	if (!checkProgramStatus(passThroughProgramID))
		return;

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	// texture shaders
	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	temp = readShaderCode("Tex_VertexShader.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("TexFragShader.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	textureProgramID = glCreateProgram();
	glAttachShader(textureProgramID, vertexShaderID);
	glAttachShader(textureProgramID, fragmentShaderID);

	glLinkProgram(textureProgramID);

	if (!checkProgramStatus(textureProgramID))
		return;

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

void MeGlWindow::initializeGL()
{
	setMouseTracking(true);
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	sendDataToOpenGL();
	installShaders();
}

MeGlWindow::~MeGlWindow()
{
	glDeleteBuffers(1, &theBufferID);
	glUseProgram(0);
	glDeleteProgram(programID);
	glDeleteProgram(passThroughProgramID); 
	glDeleteProgram(textureProgramID);
}