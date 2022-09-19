#include <gl\glew.h>
#include <MeGlWindow.h>

void MeGlWindow::initializeGL()
{
	glewInit();

	GLfloat verts[] =
	{
		/*+0.0f, +0.0f,
		+1.0f, +1.0f,
		-1.0f, +1.0f,
		-1.0f, -1.0f,
		+1.0f, -1.0f,*/

		-1.0f, +0.0f,
		-0.8f, -0.2f,
		+0.6f, +0.0f,
		+0.6f, -0.2f,
		+0.6f, +0.1f,
		+0.6f, -0.3f,
		+0.65f, +0.1f,
		+0.65f, -0.3f,
		+0.65f, +0.0f,
		+0.65f, -0.2f,
		+1.0f, +0.0f,
		+1.0f, -0.2f,
	};
	GLuint myBufferID;
	glGenBuffers(1, &myBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts),
		verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	GLushort indices[] = { 0, 1, 2, 1, 2, 3, 4, 5, 6, 5, 6, 7, 8, 9, 10, 9, 10, 11 };
	GLuint indexBufferID;
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),
		indices, GL_STATIC_DRAW);
}

void MeGlWindow::paintGL()
{
	glViewport(0, 0, width(), height());
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);
}