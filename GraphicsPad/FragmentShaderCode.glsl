#version 430

out vec4 finalColor;
in vec3 vColor;
in vec2 vPosition;

void main()
{
	finalColor = vec4(
		vColor.x * (1 - vPosition.x),
		vColor.y * vPosition.y,
		vColor.z, 1.0);
}