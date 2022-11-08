#version 430

in vec3 vColor;
in vec4 vPosition;

out vec4 finalColor;

void main()
{
	finalColor = vec4(
		vColor.x * (1 - vPosition.x),
		vColor.y * vPosition.y,
		vColor.z, 1.0);
}