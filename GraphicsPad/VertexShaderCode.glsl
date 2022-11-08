#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec2 offset;

uniform mat4 myTransformMatrix;

out vec3 vColor;
out vec4 vPosition;

void main()
{
	//gl_Position = vec4(position + offset, 0.0, 1.0);
	gl_Position = myTransformMatrix * vec4(position, 1.0);
	vColor = vertexColor;
	vPosition = gl_Position;
}