#version 430

in layout(location=0) vec2 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec2 offset;

out vec3 vColor;
out vec2 vPosition;

void main()
{
	gl_Position = vec4(position + offset, 0.0, 1.0);
	vColor = vertexColor;
	vPosition = position + offset;
}