#include "Shaders/Lighting.hlsl"

void UberVP(
	float4 position	: POSITION,
	float2 uv		: TEXCOORD0,
	float3 normal	: NORMAL,
	float3 tangent	: TANGENT0,
	
	out float4 oPosition	: POSITION,
	out float2 oUv			: TEXCOORD0,
	out float3 oLightDir	: TEXCOORD1,
	out float3 oEyeDir		: TEXCOORD2,
	out float3 oNormal		: TEXCOORD3,
	out float3 oTangent		: TEXCOORD4,
	out float3 oBitangent	: TEXCOORD5,
	out float2 oDepth		: TEXCOORD6,
	
	uniform float4x4 worldViewProj,
    uniform float4 lightPosition,
	uniform float3 eyePosition,
	uniform float4x4 world
	) {
	oPosition = mul(worldViewProj, position);
	
	oUv = uv;
	
	oDepth.x = oPosition.z;
	
	float4 worldPos = mul(world, position);
	oLightDir = lightPosition.xyz - (position.xyz * lightPosition.w);
	
	oEyeDir = normalize(eyePosition - position.xyz);
	
	oNormal = mul(world, normal);
	oTangent = mul(world, tangent);
	oBitangent = cross(oNormal, oTangent);
	
	float3x3 rotation = transpose(float3x3(oTangent, oBitangent, oNormal));
	oNormal = rotation[2];
	oBitangent = rotation[1];
	oTangent = rotation[0];
	
#ifdef EDGE
	oDepth.y = max(dot(normalize(normal), oEyeDir), 0.0f);
#else
	oDepth.y = 0.0f;
#endif
}

void UberFP(
	float2 uv			: TEXCOORD0,
	float3 lightDir		: TEXCOORD1,
	float3 eyeDir		: TEXCOORD2,
	float3 normal		: TEXCOORD3,
	float3 tangent		: TEXCOORD4,
	float3 bitangent	: TEXCOORD5,
	float2 depth		: TEXCOORD6,
	
	out float4 oColour : COLOR,
	
	uniform float4 lightColour,
	uniform float4 ambientColour,
	uniform float4 fogColour,
	uniform float4 fogParams,
	uniform sampler2D normalMap,
	uniform sampler2D diffuseMap,
	uniform sampler2D specularMap
#ifdef EDGE
	,uniform sampler1D edgeRamp
#endif
	) {
	float4 D = tex2D(diffuseMap, uv);
	float3 N = expand(tex2D(normalMap, uv).rgb);
	float3 L = normalize(lightDir);

	float3 H = normalize(eyeDir + lightDir);
	float4 S = tex2D(specularMap, uv);
	
	float3x3 rot = float3x3(tangent, bitangent, normal);
	float3 N2 = normalize(mul(rot, N)) * 0.5f;
	
	ambientColour.rgb = lerp(ambientColour * 0.5f, ambientColour, saturate(N2.z * 0.5f + 0.5f));
	
	oColour = phong(N2, L, H, lightColour, S, S.r * 128.0f, ambientColour, D, 1.0f);
	
#ifdef EDGE
	depth.y = tex1D(edgeRamp, depth.y).x;
	oColour.rgb = oColour.rgb * depth.y;
#endif

#ifdef WALL
	const float maxWallDistance = 2.5f;
	const float minAlpha = 0.4f;
	
	float a = depth.x / maxWallDistance;
	a = smoothstep(minAlpha, 1.0f, a * a);
	a = clamp(a, minAlpha, 1.0f);
	
	oColour.a = oColour.a * a;
#endif
}