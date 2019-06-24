#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex 
{
	vec4 gl_Position;
};

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 inUVCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUVCoord;

layout(binding = 0) uniform UniformStruct
{
	mat4 mvp;
} uniformObject;

void main()
{
	gl_Position = uniformObject.mvp * vec4(pos, 1.0);
	fragUVCoord = inUVCoord;
	fragColor = color;
}