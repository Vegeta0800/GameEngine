#version 450
#extension GL_ARB_separate_shader_objects : enable

//Out struct
out gl_PerVertex 
{
	vec4 gl_Position;
};

//Incoming data
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 inUVCoord;

//Outgoing data
layout(location = 0) out vec2 fragUVCoord;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragView;
layout(location = 3) out vec3 fragLight;
layout(location = 4) out vec4 fragLightColor;
layout(location = 5) out float fragAmbientValue;
layout(location = 6) out float fragSpecularValue;

//Uniform buffer
layout(binding = 0) uniform UniformStruct
{
	mat4 model;
	mat4 view;
	mat4 projection;
	vec4 colorIn;
	vec3 lightPos;
	float ambientValue;
	float specularValue;
} uniformObject;

//Compute position per vertex using the mvp matrix
void main()
{
	gl_Position = uniformObject.projection * uniformObject.view * uniformObject.model * vec4(pos, 1.0);

	//Compute world position
	vec4 worldPos = uniformObject.model * vec4(pos, 1.0);

	//Set fragment shader values
	fragUVCoord = inUVCoord;
	fragNormal = mat3(uniformObject.view) * mat3(uniformObject.model) * normal;
	fragView = -(uniformObject.view * worldPos).xyz;
	fragLight = mat3(uniformObject.view) * (uniformObject.lightPos - vec3(worldPos));
	fragLightColor = uniformObject.colorIn;
	fragAmbientValue = uniformObject.ambientValue;
	fragSpecularValue = uniformObject.specularValue;
}