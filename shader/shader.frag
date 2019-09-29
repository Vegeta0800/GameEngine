#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 fragUVCoord;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragView;
layout(location = 3) in vec3 fragLight;
layout(location = 4) in vec3 fragLightColor;
layout(location = 5) in float fragAmbientValue;
layout(location = 6) in float fragSpecularValue;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D tex;
layout(binding = 2) uniform sampler2D normalMap;
layout(binding = 3) uniform sampler2D emissionMap;
layout(binding = 4) uniform sampler2D roughnessMap;
layout(binding = 5) uniform sampler2D ambientMap;

vec4 SRGBtoLinear(vec4 srgbIn)
{
	#ifdef MANUAL_SRGB
	#ifdef SRGB_FAST_APPROXIMATION
	vec3 linOut = pow(srgbIn.xyz,vec3(2.2));
	#else //SRGB_FAST_APPROXIMATION
	vec3 bLess = step(vec3(0.04045),srgbIn.xyz);
	vec3 linOut = mix( srgbIn.xyz/vec3(12.92), pow((srgbIn.xyz+vec3(0.055))/vec3(1.055),vec3(2.4)), bLess );
	#endif //SRGB_FAST_APPROXIMATION
	return vec4(linOut,srgbIn.w);;
	#else //MANUAL_SRGB
	return srgbIn;
	#endif //MANUAL_SRGB
}

void main()
{
	vec3 N = normalize(texture(normalMap, fragUVCoord).xyz);
	vec3 V = normalize(fragView);
	vec3 L = normalize(fragLight);
	vec3 R = reflect(-L, N);

	float cosTheta = clamp( dot( N,L ), 0, 1 );
	float cosAlpha = clamp( dot( V,R ), 0, 1 );

	vec3 ambient = SRGBtoLinear(texture(ambientMap, fragUVCoord)).rgb * fragAmbientValue;
	vec3 diffuse = SRGBtoLinear(texture(tex, fragUVCoord)).rgb * fragLightColor.rgb * cosTheta;
	vec3 specular = SRGBtoLinear(texture(roughnessMap, fragUVCoord)).rgb * fragLightColor.rgb * pow(cosAlpha, fragSpecularValue);

	vec3 color = ambient + diffuse + specular;

	vec3 emissive = SRGBtoLinear(texture(emissionMap, fragUVCoord)).xyz;
	emissive =  mix(emissive, emissive * SRGBtoLinear(texture(tex, fragUVCoord)).rgb, 1.4f);

	color += emissive;

	outColor = vec4(color * fragLightColor, SRGBtoLinear(texture(tex, fragUVCoord)).a);
}