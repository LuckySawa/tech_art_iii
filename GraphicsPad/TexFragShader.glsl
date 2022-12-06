#version 430

out vec4 daColor;
in vec3 vertexPositionWorld;
in vec2 texcoord;
in mat4 _modelToWorldMatrix;

uniform vec3 lightPositionWorld;
uniform vec3 eyePositionWorld;
uniform vec4 ambientLight;


uniform sampler2D normalMap;
//uniform sampler2D baseColor;

void main()
{
	// Normal from map
	vec3 normalTS = vec3(texture(normalMap, texcoord));
	normalTS = mat3(transpose(inverse(_modelToWorldMatrix))) * normalTS;

	//vec3 baseColor = vec3(texture(baseColor, texcoord));

	// Diffuse
	vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
	float brightness = max(dot(lightVectorWorld, normalize(normalTS)),0.0);
	vec4 diffuseLight = vec4(brightness, brightness, brightness, 1.0);

	// Specular
	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normalTS);
	vec3 eyeVectorWorld = normalize(eyePositionWorld - vertexPositionWorld);
	float s = max(dot(reflectedLightVectorWorld, eyeVectorWorld),0.0);
	s = pow(s, 50);
	vec4 specularLight = vec4(1.0, 0.0, 0.0, 1) * s;

	daColor = vec4(ambientLight.xyz + clamp(diffuseLight, 0, 1).xyz + specularLight.xyz, 1.0);
	daColor = vec4(normalTS, 1.0);
}