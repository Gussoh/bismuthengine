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
	
	uniform sampler2D diffuseTexture,
	uniform sampler1D edgeRamp,
	uniform sampler1D diffuseRamp
) {
	const float maxWallDistance = 2.5f;
	const float minAlpha = 0.4f;

	float3 c = tex2D(diffuseTexture, uv).rgb;
	
	float d = tex1D(diffuseRamp, diffuse).x;
	c = diffuse * d * c;

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
	
	float d = tex1D(diffuseRamp, diffuse).x;
	c = diffuse * d * c;
	
	edge = tex1D(edgeRamp, edge).x;

	oColor = float4(c, 1.0f);
}