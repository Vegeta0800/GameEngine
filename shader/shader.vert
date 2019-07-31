#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex 
{
	vec4 gl_Position;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 inUVCoord;

layout(location = 0) out vec2 fragUVCoord;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragView;
layout(location = 3) out vec3 fragLight;
layout(location = 4) out vec4 fragColor;
layout(location = 5) out vec4 fragSpecularColor;
layout(location = 6) out float fragAmbientValue;
layout(location = 7) out float fragSpecularValue;

layout(binding = 0) uniform UniformStruct
{
	mat4 model;
	mat4 view;
	mat4 projection;
	vec4 colorIn;
	vec4 specularColor;
	vec3 lightPos;
	float ambientValue;
	float specularValue;
} uniformObject;

void main()
{
	gl_Position = uniformObject.projection * uniformObject.view * uniformObject.model * vec4(pos, 1.0);

	vec4 worldPos = uniformObject.model * vec4(pos, 1.0);

	fragUVCoord = inUVCoord;
	fragNormal = mat3(uniformObject.view) * mat3(uniformObject.model) * normal;
	fragView = -(uniformObject.view * worldPos).xyz;
	fragLight = mat3(uniformObject.view) * (uniformObject.lightPos - vec3(worldPos));
	fragColor = uniformObject.colorIn;
	fragSpecularColor = uniformObject.specularColor;
	fragAmbientValue = uniformObject.ambientValue;
	fragSpecularValue = uniformObject.specularValue;
}