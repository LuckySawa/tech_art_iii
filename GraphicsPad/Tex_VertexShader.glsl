#version 430

in layout(location=0) vec4 vertexPositionModel;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normalModel;
in layout(location=3) vec3 inTexcoord;

uniform mat4 modelToProjectionMatrix;
uniform mat4 modelToWorldMatrix;

out vec3 vertexPositionWorld;
out vec2 texcoord;
out mat4 _modelToWorldMatrix;

void main()
{
	gl_Position = modelToProjectionMatrix * vertexPositionModel;
	vertexPositionWorld = vec3(modelToWorldMatrix * vertexPositionModel);
	texcoord = vec2(inTexcoord.x, inTexcoord.y);
	_modelToWorldMatrix = modelToWorldMatrix;

}