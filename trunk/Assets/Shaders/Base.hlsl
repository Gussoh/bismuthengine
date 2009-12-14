void WallVP(
	float4 pos : POSITION,
	float3 normal : NORMAL,
	float2 uv : TEXCOORD0,
	
	out float4 oPos : POSITION,
	out float2 oUv : TEXCOORD0,
	out float oWallDistance : TEXCOORD1,
	out float oEdge : TEXCOORD2,
	out float oDiffuse : TEXCOORD3,
	
	uniform float4x4 worldViewProj,
	uniform float4 cameraPos,
	uniform float3 lightPosition
) {
	oPos = mul(worldViewProj, pos);
	oUv = uv;
		
	// Get distance from the camera to the vertex
	oWallDistance = oPos.z;
	
	float3 N = normalize(normal);
	float3 E = normalize(cameraPos - pos.xyz);
	oEdge = max(dot(N, E), 0.0f);
	
	float3 L = normalize(lightPosition - pos.xyz);
	oDiffuse = max(dot(N, L), 0.0f);
}

void WallFP(
	float2 uv : TEXCOORD0,
	float wallDistance : TEXCOORD1,
	float edge : TEXCOORD2,
	float diffuse : TEXCOORD3,
	
	out float4 oColor : COLOR,
	
	//uniform float maxWallDistance,
	//uniform float minAlpha,
	
	uniform sampler2D diffuseTexture : register(s0),
	uniform sampler1D edgeRamp : register(s1),
	uniform sampler1D diffuseRamp : register(s2)
) {
	const float maxWallDistance = 2.5f;
	const float minAlpha = 0.4f;

	float3 c = tex2D(diffuseTexture, uv).rgb;
	
	float d = 0.0f;
	if (diffuse < 0.2f) {
		d = 0.2f;
	} else if (diffuse < 0.4f) {
		d = 0.4f;
	} else if (diffuse < 0.6f) {
		d = 0.6f;
	} else if (diffuse < 0.8f) {
		d = 0.8f;
	} else {
		d = 1.0f;
	}
	c = d * c;

	// Fragments closer to the camera are more transparent
	float a = wallDistance / maxWallDistance;
	a = smoothstep(minAlpha, 1.0f, a * a);
	a = clamp(a, minAlpha, 1.0f);
	
	edge = tex1D(edgeRamp, edge).x;

	oColor = float4(c, a);
}

void CellVP(
	float4 pos : POSITION,
	float3 normal : NORMAL,
	float2 uv : TEXCOORD0,
	
	out float4 oPos : POSITION,
	out float2 oUv : TEXCOORD0,
	out float oEdge : TEXCOORD1,
	out float oDiffuse : TEXCOORD2,
	
	uniform float4x4 worldViewProj,
	uniform float4 cameraPos,
	uniform float3 lightPosition
) {
	oPos = mul(worldViewProj, pos);
	oUv = uv;
	
	float3 N = normalize(normal);
	float3 E = normalize(cameraPos - pos.xyz);
	oEdge = max(dot(N, E), 0.0f);
	
	float3 L = normalize(lightPosition - pos.xyz);
	oDiffuse = max(dot(N, L), 0.0f);
}

void CellFP(
	float2 uv : TEXCOORD0,
	float edge : TEXCOORD1,
	float diffuse : TEXCOORD2,
	
	out float4 oColor : COLOR,
	
	//uniform float maxWallDistance,
	//uniform float minAlpha,
	
	uniform sampler2D diffuseTexture,
	uniform sampler1D edgeRamp,
	uniform sampler1D diffuseRamp
) {
	float3 c = tex2D(diffuseTexture, uv).rgb;
	
	float d = 0.0f;
	if (diffuse < 0.2f) {
		d = 0.2f;
	} else if (diffuse < 0.4f) {
		d = 0.4f;
	} else if (diffuse < 0.6f) {
		d = 0.6f;
	} else if (diffuse < 0.8f) {
		d = 0.8f;
	} else {
		d = 1.0f;
	}
	
	c = d * c;

	edge = tex1D(edgeRamp, edge).x;
	
#ifdef EDGE
	c = c * edge;
#endif

	oColor = float4(c, 1.0f);
}









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
#ifdef SHADOWS
	out float4 oShadowUv	: TEXCOORD3,
#endif
	out float3 oNormal		: TEXCOORD4,
	out float3 oTangent		: TEXCOORD5,
	out float3 oBitangent	: TEXCOORD6,
	out float  oDepth		: TEXCOORD7,
	
	uniform float4x4 worldViewProj,
    uniform float4 lightPosition,
	uniform float3 eyePosition,
	uniform float4x4 world
#ifdef SHADOWS
	,uniform float4x4 texViewProj1
#endif
	) {
	oPosition = mul(worldViewProj, position);
	
	oUv = uv;
	
	oDepth = length(position.xyz - eyePosition);
	
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
	
#ifdef SHADOWS
    oShadowUv  = mul(texViewProj1, worldPos);
#endif
}

void UberFP(
	float2 uv			: TEXCOORD0,
	float3 lightDir		: TEXCOORD1,
	float3 eyeDir		: TEXCOORD2,
#ifdef SHADOWS
	float4 shadowUv1	: TEXCOORD3,
#endif
	float3 normal		: TEXCOORD4,
	float3 tangent		: TEXCOORD5,
	float3 bitangent	: TEXCOORD6,
	float  depth		: TEXCOORD7,
	
	out float4 oColour : COLOR,
	
	uniform float4 lightColour,
	uniform float4 ambientColour,
	uniform float4 fogColour,
	uniform float4 fogParams,
#ifdef MODEL_OREN_NAYAR
	uniform float roughness,
#endif
#ifdef OMNI
	uniform float4 lightAttenuation,
#endif
	
	uniform sampler2D normalMap,
	uniform sampler2D diffuseMap,
#ifdef OREN_NAYAR_USE_LOOKUP
	uniform sampler2D orenLookup
#else
	uniform sampler2D specularMap
#endif
#ifdef CUBE_MAP
	,uniform samplerCUBE skyMap
#endif
#ifdef SHADOWS
	,uniform sampler2D shadowMap1
#endif
	) {
	float4 D = tex2D(diffuseMap, uv);
	float3 N = expand(tex2D(normalMap, uv).rgb);
	float3 L = normalize(lightDir);

#ifdef MODEL_OREN_NAYAR
	float3 V = normalize(eyeDir);
#ifdef OREN_NAYAR_USE_LOOKUP
	oColour = orenNayar(N, L, V, lightColour, roughness, ambientColour, D, orenLookup);
#else
	oColour = orenNayar(N, L, V, lightColour, roughness, ambientColour, D);
#endif
#else
	float3 H = normalize(eyeDir + lightDir);
	float4 S = tex2D(specularMap, uv);
	
	float shadowAmount = 1;
#ifdef SHADOWS
	shadowAmount = min(shadow(shadowMap1, shadowUv1) + 0.25f, 1.0f);
#endif
	
#ifdef CUBE_MAP	
	D.rgb += texCUBE(skyMap, reflect(eyeDir, N)).rgb * S.b;
#endif

	float3x3 rot = float3x3(tangent, bitangent, normal);
	float3 N2 = normalize(mul(rot, N)) * 0.5f;
	
	ambientColour.rgb = lerp(ambientColour * 0.5f, ambientColour, saturate(N2.z * 0.5f + 0.5f));
	
	oColour = phong(N2, L, H, lightColour, S, S.r * 128.0f, ambientColour, D, shadowAmount);
	
#ifdef OMNI
	float attenuation = length(lightDir) / lightAttenuation.x;
	attenuation = saturate(1.0f - (attenuation * attenuation));
	
	oColour = oColour * attenuation;
#endif
	
	
#endif
}

void SimpleVP(
	float4 position : POSITION,
	float2 uv		: TEXCOORD0,
	float3 normal	: NORMAL,
	
	out float4 oPosition	: POSITION,
	out float2 oUv			: TEXCOORD0,
	out float3 oNormal		: TEXCOORD1,
	out float  oDepth		: TEXCOORD2,
	out float3 oDiffuse		: TEXCOORD3,
	
	uniform float4x4 worldViewProj,
	uniform float3 eyePosition,
	uniform float4x4 world,
	uniform float4 lightPosition,
	uniform float4 lightColour
	) {
	oPosition = mul(worldViewProj, position);
	oUv = uv;
	oNormal = normalize(mul(world, normal));
	oDepth = length(position.xyz - eyePosition);
	
	float3 lightDir = normalize(lightPosition.xyz - (position.xyz * lightPosition.w));
	oDiffuse = lightColour.xyz * dot(normalize(normal), lightDir);
}

void SimpleFP(
	float2 uv		: TEXCOORD0,
	float3 normal	: TEXCOORD1,
	float  depth	: TEXCOORD2,
	float3 diffuse	: TEXCOORD3,
	
	out float4 oColour : COLOR,
	
	uniform float4 ambientColour,
	uniform float4 fogColour,
	uniform float4 fogParams,
	uniform sampler2D tex
	) {
	ambientColour.rgb = lerp(ambientColour * 0.5f, ambientColour, saturate(normal.z * 0.5f + 0.5f));
	oColour = tex2D(tex, uv);
	oColour.rgb = (ambientColour.rgb + diffuse) * oColour.rgb;
	oColour.rgb = lerp(oColour.rgb, fogColour, saturate((depth - fogParams.y) / (fogParams.z - fogParams.y)));
}