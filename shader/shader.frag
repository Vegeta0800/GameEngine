#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 fragUVCoord;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragView;
layout(location = 3) in vec3 fragLight;
layout(location = 4) in vec3 fragColor;
layout(location = 5) in vec3 fragSpecularColor;
layout(location = 6) in float fragAmbientValue;
layout(location = 7) in float fragSpecularValue;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D tex;


void main()
{
	//texture(tex, fragUVCoord)

	vec3 N = normalize(fragNormal);
	vec3 V = normalize(fragView);
	vec3 L = normalize(fragLight);
	vec3 R = reflect(L, N);

	vec3 ambient = texture(tex, fragUVCoord).xyz * 0.1;
	vec3 diffuse = max(dot(N, L), 0.0) * texture(tex, fragUVCoord).xyz;
	vec3 specular = pow(max(dot(R, V), 0.0), 16.0) * vec3(1.0, 1.0, 1.0);

	outColor = vec4(ambient + diffuse + specular, 1.0);
}